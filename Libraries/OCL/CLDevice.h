#ifndef CLDEVICE_H__
#define CLDEVICE_H__

#include <vector>
#include <string>
#include <OpenCL/cl.h>

#include "CLPlatform.h"

class CLDevice
{
    friend class CLCommandQueue;
    friend class CLContext;
    friend class CLProgram;
    
public:
    CLDevice(const CLDevice& device);
    virtual ~CLDevice();
    static std::vector<CLDevice> list(const CLPlatform& platform, cl_device_type type);
    std::string info(cl_device_info name) const;
    
private:
    CLDevice(cl_device_id device_id);
    
    cl_device_id m_device_id;
    
};

#endif
