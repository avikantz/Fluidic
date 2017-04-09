#include "FluidRendererCPP.h"

#include <GLKit/GLKMatrix4.h>
#include <OpenGL/gl3.h>

#include <OpenCL/cl_gl_ext.h>
#include <OpenGL/CGLContext.h>
#include <OpenGL/CGLCurrent.h>
#include <OpenGL/CGLDevice.h>

#include <CoreFoundation/CFDate.h>

#include <iostream>
#include <iomanip>
#include <math.h>

#define NUM_PARTICLES_AXIS 24
#define NUM_PARTICLES (NUM_PARTICLES_AXIS * NUM_PARTICLES_AXIS * NUM_PARTICLES_AXIS)
#define GRID_SIZE 32
#define STRINGIFY(str) #str

const GLchar* VERTEX_SOURCE = "#version 410\n"
STRINGIFY(
          layout(location = 0) in vec4 position;
          layout(location = 1) in vec2 particlePosition;
          uniform mat4 viewMatrix;
          uniform mat4 projMatrix;
          uniform float particleScale;
          out vec2 fragPosition;
          out vec4 eyePosition;
          void main()
          {
              fragPosition = particlePosition;
              eyePosition = viewMatrix * position;
              eyePosition /= eyePosition.w;
              gl_Position = eyePosition;
              gl_Position.xy += particlePosition * particleScale;
              gl_Position = projMatrix * gl_Position;
          }
          );

const GLchar* FRAGMENT_SOURCE = "#version 410\n"
STRINGIFY(
          uniform mat4 projMatrix;
          uniform float particleScale;
          in vec2 fragPosition;
          in vec4 eyePosition;
          out vec4 fragColor;
          void main()
          {
              const vec3 outColor = vec3(1.0, 0.6, 0.7);
              const vec3 lightDir = vec3(0.500, 0.500, 1.500);
              
              float r2 = length(fragPosition);
              float nz = sqrt(1.0 - r2);
              float diffuse = max(0.3, dot(lightDir, vec3(fragPosition, nz)));
              diffuse = 0.25 + floor(diffuse * 3.0) * 0.25;
              fragColor = vec4(outColor * diffuse, 1.0);
              
              vec4 pixelNormal = vec4(fragPosition, nz, 0.0);
              vec4 pixelPos = eyePosition + pixelNormal * particleScale;
              vec4 clipSpacePos = projMatrix * pixelPos;
              gl_FragDepth = clipSpacePos.z / clipSpacePos.w * 0.5 + 0.5;
              
              if (r2 > 1.0) discard;
          }
          );

const char* CL_KERNEL_SOURCE =
#include "Fluid.cl"
;

struct FluidVars
{
    cl_float timeDiff             = 0.003;
    cl_float restDensity          = 300.0;
    cl_float internalStiffness    = 1.5;
    cl_float viscosity            = 0.45;
    
    cl_float boundaryStiffness    = 10000.0;
    cl_float boundaryDampening    = 256.0;
    cl_float3 boundaryMin         = { 0.0, 0.0, 0.0 };
    cl_float3 boundaryMax         = { 1.0, 1.0, 0.6 };
    
    cl_float3 initMin             = { 0.8, 0.4, -.3 };
    cl_float3 initMax             = { 1.0, 1.0, 0.3 };
    
    cl_float accelerationLimit    = 150.0;
    cl_float velocityLimit        = 600.0;
    cl_float simulationScale      = 1.0 / fmax(boundaryMax.x - boundaryMin.x,
                                               fmax(boundaryMax.y - boundaryMin.y, boundaryMax.z - boundaryMin.z));
    
    cl_float3 gravity             = { 0.0, -6.0, 0.0 };
    
    cl_float particleMass         = 0.00020543 * 10;
    cl_float smoothingRadius      = 0.02;
    cl_float boundaryRadius       = 0.04;
    
    cl_float d2                   = simulationScale * simulationScale;
    cl_float rd2                  = smoothingRadius * smoothingRadius / d2;
    cl_float poly6Kern            = 315.0 / (64.0 * M_PI * pow(smoothingRadius, 9.0));
    cl_float spikyKern            = -45.0 / (M_PI * pow(smoothingRadius, 6.0));
    cl_float lapKern              = 45.0 / (M_PI * pow(smoothingRadius, 6.0));
    
    cl_uint gridSize              = GRID_SIZE;
    cl_uint gridCount             = GRID_SIZE * GRID_SIZE * GRID_SIZE;
    
} fluidVars;

FluidRendererCPP::FluidRendererCPP()
{
    GLfloat PARTICLE_VERTICES[8] = { -1, 1, -1, -1, 1, 1, 1, -1 };
    _glProgram = std::unique_ptr<GLProgram>(new GLProgram(VERTEX_SOURCE, FRAGMENT_SOURCE));
    _glVAO = std::unique_ptr<GLVertexArray>(new GLVertexArray());
    _glVertices = std::unique_ptr<GLBuffer>
    (new GLBuffer(GL_ARRAY_BUFFER, 4 * NUM_PARTICLES * sizeof(GLfloat), nullptr, GL_STATIC_DRAW));
    _glParticleVertices = std::unique_ptr<GLBuffer>
    (new GLBuffer(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), PARTICLE_VERTICES, GL_STATIC_DRAW));
    
    _glVAO->bind();
    _glVertices->bind();
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribDivisor(0, 1);
    glEnableVertexAttribArray(0);
    _glVertices->unbind();
    _glParticleVertices->bind();
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribDivisor(1, 0);
    glEnableVertexAttribArray(1);
    _glParticleVertices->unbind();
    _glVAO->unbind();
    
    CLPlatform clPlatform = CLPlatform::list()[0];
    CLDevice clDevice = CLDevice::list(clPlatform, CL_DEVICE_TYPE_GPU)[0];
    
    // Get current CGL Context and CGL Share group
    CGLContextObj kCGLContext = CGLGetCurrentContext();
    CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
    // Create CL context properties, add handle & share-group enum
    cl_context_properties clProperties[] = {
        CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
        (cl_context_properties)kCGLShareGroup, 0
    };
    
    _clContext = std::unique_ptr<CLContext>(new CLContext(clProperties, clDevice));
    _clCommandQueue = std::unique_ptr<CLCommandQueue>(new CLCommandQueue(*_clContext, clDevice));
    _clProgram = std::unique_ptr<CLProgram>(new CLProgram(*_clContext, clDevice, CL_KERNEL_SOURCE));
    
    _clBufferVAO = std::unique_ptr<CLBufferShared>
    (new CLBufferShared(*_clContext, CL_MEM_WRITE_ONLY, _glVertices->name()));
    
    _clBufferPosition = std::unique_ptr<CLBuffer>
    (new CLBuffer(*_clContext, CL_MEM_HOST_NO_ACCESS, NUM_PARTICLES * sizeof(cl_float3), nullptr));
    _clBufferPositionSort = std::unique_ptr<CLBuffer>
    (new CLBuffer(*_clContext, CL_MEM_HOST_READ_ONLY, NUM_PARTICLES * sizeof(cl_float3), nullptr));
    _clBufferVelocity = std::unique_ptr<CLBuffer>
    (new CLBuffer(*_clContext, CL_MEM_HOST_NO_ACCESS, NUM_PARTICLES * sizeof(cl_float3), nullptr));
    _clBufferVelocitySort = std::unique_ptr<CLBuffer>
    (new CLBuffer(*_clContext, CL_MEM_HOST_NO_ACCESS, NUM_PARTICLES * sizeof(cl_float3), nullptr));
    _clBufferVelocityEval = std::unique_ptr<CLBuffer>
    (new CLBuffer(*_clContext, CL_MEM_HOST_NO_ACCESS, NUM_PARTICLES * sizeof(cl_float3), nullptr));
    _clBufferVelocityEvalSort = std::unique_ptr<CLBuffer>
    (new CLBuffer(*_clContext, CL_MEM_HOST_NO_ACCESS, NUM_PARTICLES * sizeof(cl_float3), nullptr));
    _clBufferForce = std::unique_ptr<CLBuffer>
    (new CLBuffer(*_clContext, CL_MEM_HOST_NO_ACCESS, NUM_PARTICLES * sizeof(cl_float3), nullptr));
    _clBufferPressure = std::unique_ptr<CLBuffer>
    (new CLBuffer(*_clContext, CL_MEM_HOST_NO_ACCESS, NUM_PARTICLES * sizeof(cl_float), nullptr));
    _clBufferDensity = std::unique_ptr<CLBuffer>
    (new CLBuffer(*_clContext, CL_MEM_HOST_NO_ACCESS, NUM_PARTICLES * sizeof(cl_float), nullptr));
    
    _clBufferGridCount = std::unique_ptr<CLBuffer>
    (new CLBuffer(*_clContext, CL_MEM_HOST_NO_ACCESS, GRID_SIZE * GRID_SIZE * GRID_SIZE * sizeof(cl_uint), nullptr));
    _clBufferGridScan = std::unique_ptr<CLBuffer>
    (new CLBuffer(*_clContext, CL_MEM_HOST_NO_ACCESS, GRID_SIZE * GRID_SIZE * GRID_SIZE * sizeof(cl_uint), nullptr) );
    
    _clKernelCopy = std::unique_ptr<CLKernel>(new CLKernel(*_clProgram, "copy"));
    _clKernelCopy->setArg(0, *_clBufferPosition);
    _clKernelCopy->setArg(1, *_clBufferVAO);
    
    _clKernelPressure = std::unique_ptr<CLKernel>(new CLKernel(*_clProgram, "pressure"));
    _clKernelPressure->setArg(0, *_clBufferGridScan);
    _clKernelPressure->setArg(1, *_clBufferGridCount);
    _clKernelPressure->setArg(2, *_clBufferPositionSort);
    _clKernelPressure->setArg(3, *_clBufferPressure);
    _clKernelPressure->setArg(4, *_clBufferDensity);
    _clKernelPressure->setArg(5, sizeof(FluidVars), &fluidVars);
    
    _clKernelForce = std::unique_ptr<CLKernel>(new CLKernel(*_clProgram, "force"));
    _clKernelForce->setArg(0, *_clBufferGridScan);
    _clKernelForce->setArg(1, *_clBufferGridCount);
    _clKernelForce->setArg(2, *_clBufferPositionSort);
    _clKernelForce->setArg(3, *_clBufferVelocityEvalSort);
    _clKernelForce->setArg(4, *_clBufferPressure);
    _clKernelForce->setArg(5, *_clBufferDensity);
    _clKernelForce->setArg(6, *_clBufferForce);
    _clKernelForce->setArg(7, sizeof(FluidVars), &fluidVars);
    
    _clKernelAdvance = std::unique_ptr<CLKernel>(new CLKernel(*_clProgram, "advance"));
    _clKernelAdvance->setArg(0, *_clBufferPositionSort);
    _clKernelAdvance->setArg(1, *_clBufferVelocitySort);
    _clKernelAdvance->setArg(2, *_clBufferVelocityEvalSort);
    _clKernelAdvance->setArg(3, *_clBufferForce);
    _clKernelAdvance->setArg(4, *_clBufferPosition);
    _clKernelAdvance->setArg(5, *_clBufferVelocity);
    _clKernelAdvance->setArg(6, *_clBufferVelocityEval);
    _clKernelAdvance->setArg(7, sizeof(FluidVars), &fluidVars);
    
    cl_uint particleCount = NUM_PARTICLES;
    _clKernelGridSort = std::unique_ptr<CLKernel>(new CLKernel(*_clProgram, "gridSort"));
    _clKernelGridSort->setArg(0, *_clBufferPosition);
    _clKernelGridSort->setArg(1, *_clBufferVelocity);
    _clKernelGridSort->setArg(2, *_clBufferVelocityEval);
    _clKernelGridSort->setArg(3, *_clBufferPositionSort);
    _clKernelGridSort->setArg(4, *_clBufferVelocitySort);
    _clKernelGridSort->setArg(5, *_clBufferVelocityEvalSort);
    _clKernelGridSort->setArg(6, *_clBufferGridCount);
    _clKernelGridSort->setArg(7, *_clBufferGridScan);
    _clKernelGridSort->setArg(8, sizeof(cl_uint), &particleCount);
    _clKernelGridSort->setArg(9, sizeof(FluidVars), &fluidVars);
    
    size_t globalSize[3] = { NUM_PARTICLES_AXIS, NUM_PARTICLES_AXIS, NUM_PARTICLES_AXIS };
    size_t localSize[3] = { 4, 4, 4 };
    CLKernel clKernelInit(*_clProgram, "init");
    clKernelInit.setArg(0, *_clBufferPosition);
    clKernelInit.setArg(1, *_clBufferVelocity);
    clKernelInit.setArg(2, *_clBufferVelocityEval);
    clKernelInit.setArg(3, sizeof(FluidVars), &fluidVars);
    _clCommandQueue->enqueueNDRangeKernel(clKernelInit, 3, nullptr, globalSize, localSize);
}

FluidRendererCPP::~FluidRendererCPP()
{
}

void FluidRendererCPP::render(CGSize size)
{
    glViewport(0, 0, size.width, size.height);
    
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    CFDateRef timeStart = CFDateCreate(nullptr, CFAbsoluteTimeGetCurrent());
    CFDateRef timeStartOper;
    
    size_t globalSizeGridSort[1] = { 256 };
    size_t localSizeGridSort[1] = { 256 };
    
    timeStartOper = CFDateCreate(nullptr, CFAbsoluteTimeGetCurrent());
    
    _clCommandQueue->enqueueNDRangeKernel(*_clKernelGridSort, 1, nullptr, globalSizeGridSort, localSizeGridSort);
    
    _clCommandQueue->finish();
    double timeSort = CFDateGetTimeIntervalSinceDate(CFDateCreate(nullptr, CFAbsoluteTimeGetCurrent()), timeStartOper);
    std::cout << "time sort = " << std::fixed << std::setprecision(6) << timeSort;
    
    size_t globalSize[1] = { NUM_PARTICLES };
    size_t localSize[1] = { 16 };
    
    timeStartOper = CFDateCreate(nullptr, CFAbsoluteTimeGetCurrent());
    
    _clCommandQueue->enqueueNDRangeKernel(*_clKernelPressure, 1, nullptr, globalSize, localSize);
    _clCommandQueue->enqueueNDRangeKernel(*_clKernelForce, 1, nullptr, globalSize, localSize);
    _clCommandQueue->enqueueNDRangeKernel(*_clKernelAdvance, 1, nullptr, globalSize, localSize);
    
    _clCommandQueue->finish();
    double timeSPH = CFDateGetTimeIntervalSinceDate(CFDateCreate(nullptr, CFAbsoluteTimeGetCurrent()), timeStartOper);
    std::cout << "  sph = " << std::fixed << std::setprecision(6) << timeSPH;
    
    _clCommandQueue->enqueueAcquireGLBuffer(*_clBufferVAO);
    _clCommandQueue->enqueueNDRangeKernel(*_clKernelCopy, 1, nullptr, globalSize, localSize);
    _clCommandQueue->enqueueReleaseGLBuffer(*_clBufferVAO);
    _clCommandQueue->finish();
    
    double timeFull = CFDateGetTimeIntervalSinceDate(CFDateCreate(nullptr, CFAbsoluteTimeGetCurrent()), timeStart);
    std::cout << "  full = " << std::fixed << std::setprecision(6) << timeFull;
    std::cout << "  fps = " << std::fixed << std::setprecision(2) << 1.0 / timeFull << std::endl;
    
    GLKMatrix4 tranMatrix = GLKMatrix4MakeTranslation((fluidVars.boundaryMin.x + fluidVars.boundaryMax.x) * -0.5,
                                                      (fluidVars.boundaryMin.y + fluidVars.boundaryMax.y) * -0.5,
                                                      (fluidVars.boundaryMin.z + fluidVars.boundaryMax.z) * -0.5);
    GLKMatrix4 lookMatrix = GLKMatrix4MakeLookAt(0.8, 0.4, 0.6, 0.3, -0.25, 0.1, 0.0, 0.8, 0.0);
    GLKMatrix4 viewMatrix = GLKMatrix4Multiply(lookMatrix, tranMatrix);
    GLKMatrix4 projMatrix = GLKMatrix4MakePerspective(2 * M_PI * 50.0 / 360.0,
                                                      size.width / size.height,
                                                      0.1, 5.0f);
    
    _glProgram->use();
    _glVAO->bind();
    glEnable(GL_DEPTH_TEST);
    glUniformMatrix4fv(_glProgram->uniform("viewMatrix"), 1, GL_FALSE, viewMatrix.m);
    glUniformMatrix4fv(_glProgram->uniform("projMatrix"), 1, GL_FALSE, projMatrix.m);
    glUniform1f(_glProgram->uniform("particleScale"), 0.5 * fluidVars.smoothingRadius / fluidVars.simulationScale);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, NUM_PARTICLES);
    _glVAO->unbind();
}
