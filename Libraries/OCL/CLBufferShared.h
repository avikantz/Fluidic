#ifndef CLBUFFERSHARED_H__
#define CLBUFFERSHARED_H__

#include <OpenCL/cl.h>
#include <OpenCL/cl_gl.h>

#include "CLContext.h"

class CLBufferShared
{
    friend class CLCommandQueue;
    friend class CLKernel;
    
public:
    CLBufferShared(const CLBufferShared& copy);
    CLBufferShared(const CLContext& context, cl_mem_flags flags, cl_GLuint name);
    virtual ~CLBufferShared();
    
private:
    cl_mem m_buffer;
    
};

#endif
