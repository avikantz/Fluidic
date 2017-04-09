#include "CLMem.h"
#include <iostream>

CLMem::CLMem(cl_mem buffer) : m_mem(buffer)
{
}

CLMem::CLMem(const CLMem& copy) : m_mem(copy.m_mem)
{
    clRetainMemObject(m_mem);
}

CLMem::~CLMem()
{
    clReleaseMemObject(m_mem);
}

CLMem* CLMem::create(const CLContext& context, cl_mem_flags flags, size_t dataSz, void* data)
{
    cl_int error;
    cl_mem buffer = clCreateBuffer(context.m_context, flags, dataSz, data, &error);
    if (error != CL_SUCCESS)
    {
        std::cout << "clCreateBuffer failed" << std::endl;
    }
    return new CLMem(buffer);
}

CLMem* CLMem::createFromGLBuffer(const CLContext& context, cl_mem_flags flags, cl_GLuint name)
{
    cl_int error;
    cl_mem buffer = clCreateFromGLBuffer(context.m_context, flags, name, &error);
    if (error != CL_SUCCESS)
    {
        std::cout << "clCreateFromGLBuffer failed" << std::endl;
    }
    return new CLMem(buffer);
}

CLMem* CLMem::createFromGLTexture(const CLContext& context, cl_mem_flags flags,
                                   cl_GLenum target, cl_GLint miplevel, cl_GLuint texture)
{
    cl_int error;
    cl_mem buffer = clCreateFromGLTexture(context.m_context, flags, target, miplevel, texture, &error);
    if (error != CL_SUCCESS)
    {
        std::cout << "clCreateFromGLTexture failed" << std::endl;
    }
    return new CLMem(buffer);
}
