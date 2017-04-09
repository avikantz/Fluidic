#ifndef CLPLATFORM_H__
#define CLPLATFORM_H__

#include <vector>
#include <string>
#include <OpenCL/cl.h>

class CLPlatform {
    friend class CLDevice;
    
public:
    CLPlatform(const CLPlatform& platform);
    virtual ~CLPlatform();
    static std::vector<CLPlatform> list();
    std::string info(cl_platform_info name) const;
    
private:
    CLPlatform(cl_platform_id platform_id);
    
    cl_platform_id m_platform_id;
};

#endif
