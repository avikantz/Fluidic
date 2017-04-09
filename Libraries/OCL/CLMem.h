#ifndef CLMEM_H__
#define CLMEM_H__

#include <OpenCL/cl.h>
#include <OpenCL/cl_gl.h>

#include "CLContext.h"

class CLMem
{
    
    friend class CLCommandQueue;
    friend class CLKernel;
    
public:
    CLMem(cl_mem buffer);
    CLMem(const CLMem& copy);
    virtual ~CLMem();
    
    static CLMem* create(const CLContext& context, cl_mem_flags flags, size_t dataSz, void* data);
    static CLMem* createFromGLBuffer(const CLContext& context, cl_mem_flags flags, cl_GLuint name);
    static CLMem* createFromGLTexture(const CLContext& context, cl_mem_flags flags,
                                     cl_GLenum target, cl_GLint miplevel, cl_GLuint texture);
    
private:
    cl_mem m_mem;
    
};

#endif
