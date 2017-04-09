#ifndef FLUIDRENDERERCPP_H__
#define FLUIDRENDERERCPP_H__

#include <CoreGraphics/CGGeometry.h>
#include <memory>

#include "OGL/GLBuffer.h"
#include "OGL/GLProgram.h"
#include "OGL/GLVertexArray.h"

#include "OCL/CLBuffer.h"
#include "OCL/CLBufferShared.h"
#include "OCL/CLContext.h"
#include "OCL/CLCommandQueue.h"
#include "OCL/CLProgram.h"

class FluidRendererCPP
{
public:
    FluidRendererCPP();
    virtual ~FluidRendererCPP();
    
    void render(CGSize size);
    
    private:
    std::unique_ptr<GLProgram> _glProgram;
    std::unique_ptr<GLVertexArray> _glVAO;
    std::unique_ptr<GLBuffer> _glVertices;
    std::unique_ptr<GLBuffer> _glParticleVertices;
    
    std::unique_ptr<CLContext> _clContext;
    std::unique_ptr<CLCommandQueue> _clCommandQueue;
    std::unique_ptr<CLProgram> _clProgram;
    
    std::unique_ptr<CLBufferShared> _clBufferVAO;
    
    std::unique_ptr<CLBuffer> _clBufferPosition;
    std::unique_ptr<CLBuffer> _clBufferPositionSort;
    std::unique_ptr<CLBuffer> _clBufferVelocity;
    std::unique_ptr<CLBuffer> _clBufferVelocitySort;
    std::unique_ptr<CLBuffer> _clBufferVelocityEval;
    std::unique_ptr<CLBuffer> _clBufferVelocityEvalSort;
    std::unique_ptr<CLBuffer> _clBufferForce;
    std::unique_ptr<CLBuffer> _clBufferPressure;
    std::unique_ptr<CLBuffer> _clBufferDensity;
    
    std::unique_ptr<CLBuffer> _clBufferGridCount;
    std::unique_ptr<CLBuffer> _clBufferGridScan;
    
    std::unique_ptr<CLKernel> _clKernelCopy;
    std::unique_ptr<CLKernel> _clKernelPressure;
    std::unique_ptr<CLKernel> _clKernelForce;
    std::unique_ptr<CLKernel> _clKernelAdvance;
    
    std::unique_ptr<CLKernel> _clKernelGridSort;
    
};

#endif