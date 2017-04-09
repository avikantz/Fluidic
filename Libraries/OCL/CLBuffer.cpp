#include <iostream>
#include "CLBuffer.h"

CLBuffer::CLBuffer(const CLBuffer& copy) : m_buffer(copy.m_buffer)
{
    cl_int error = clRetainMemObject(m_buffer);
    if (error != CL_SUCCESS)
    {
        std::cout << "clRetainMemObject failed" << std::endl;
    }
}

CLBuffer::CLBuffer(const CLContext& context, cl_mem_flags flags, size_t dataSz, void* data)
{
    cl_int error;
    m_buffer = clCreateBuffer(context.m_context, flags, dataSz, data, &error);
    if (error != CL_SUCCESS)
    {
        std::cout << "clCreateBuffer failed" << std::endl;
    }
}

CLBuffer::~CLBuffer()
{
    clReleaseMemObject(m_buffer);
}
