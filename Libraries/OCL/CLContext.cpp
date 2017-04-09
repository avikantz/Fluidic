#include <iostream>
#include "CLContext.h"

CLContext::CLContext(const CLDevice& device) : CLContext(nullptr, device)
{
}

CLContext::CLContext(const cl_context_properties* properties, const CLDevice& device)
{
    cl_int error;
    m_context = clCreateContext(properties, 1, &device.m_device_id, &printError, nullptr, &error);
    if (error != CL_SUCCESS)
    {
        std::cout << "clCreateContext failed" << std::endl;
    }
}

CLContext::~CLContext()
{
    clReleaseContext(m_context);
}

void CLContext::printError(const char *errinfo, const void *private_info, size_t cb, void *user_data)
{
    std::cout << "Context Error : " << errinfo << std::endl;
}
