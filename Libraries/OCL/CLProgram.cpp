#include <iostream>
#include "CLProgram.h"

CLProgram::CLProgram(const CLContext& context, const CLDevice& device, const char* source)
{
    cl_int error;
    m_program = clCreateProgramWithSource(context.m_context, 1, &source, NULL, &error);
    if (error != CL_SUCCESS)
    {
        std::cout << "clCreateProgramWithSource" << std::endl;
    }
    error = clBuildProgram(m_program, 1, &device.m_device_id, NULL, NULL, NULL);
    if (error != CL_SUCCESS)
    {
        size_t buildLogSize;
        clGetProgramBuildInfo(m_program, device.m_device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &buildLogSize);
        std::vector<char> buildLog(buildLogSize);
        clGetProgramBuildInfo(m_program, device.m_device_id, CL_PROGRAM_BUILD_LOG, buildLogSize, buildLog.data(), NULL);
        std::cout << "clBuildProgram" << std::endl << buildLog.data() << std::endl;
    }
}

CLProgram::~CLProgram()
{
    clReleaseProgram(m_program);
}
