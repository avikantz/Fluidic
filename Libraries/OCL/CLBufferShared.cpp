#include <iostream>
#include "CLBufferShared.h"

CLBufferShared::CLBufferShared(const CLBufferShared& copy) : m_buffer(copy.m_buffer)
{
    cl_int error = clRetainMemObject(m_buffer);
    if (error != CL_SUCCESS)
    {
        std::cout << "clRetainMemObject failed" << std::endl;
    }
}

CLBufferShared::CLBufferShared(const CLContext& context, cl_mem_flags flags, cl_GLuint name)
{
    cl_int error;
    m_buffer = clCreateFromGLBuffer(context.m_context, flags, name, &error);
    if (error != CL_SUCCESS)
    {
        std::cout << "clCreateFromGLBuffer failed" << std::endl;
    }
}

CLBufferShared::~CLBufferShared()
{
    clReleaseMemObject(m_buffer);
}
