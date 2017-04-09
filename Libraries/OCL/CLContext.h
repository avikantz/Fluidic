#ifndef CLCONTEXT_H__
#define CLCONTEXT_H__

#include "CLDevice.h"

class CLContext
{
    friend class CLBuffer;
    friend class CLBufferShared;
    friend class CLCommandQueue;
    friend class CLMem;
    friend class CLProgram;
    
public:
    CLContext(const CLDevice& device);
    CLContext(const cl_context_properties* properties, const CLDevice& device);
    virtual ~CLContext();
    
private:
    static void printError(const char *errinfo, const void *private_info, size_t cb, void *user_data);
    
    cl_context m_context;
    
};

#endif
