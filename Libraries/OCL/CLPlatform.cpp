#include <iostream>
#include "CLPlatform.h"

CLPlatform::CLPlatform(cl_platform_id platform_id) : m_platform_id(platform_id)
{
}

CLPlatform::CLPlatform(const CLPlatform& platform) : m_platform_id(platform.m_platform_id)
{
}

CLPlatform::~CLPlatform()
{
}

std::vector<CLPlatform> CLPlatform::list()
{
    cl_uint platformCount = 0;
    cl_int error = clGetPlatformIDs(0, NULL, &platformCount);
    if (error != CL_SUCCESS)
    {
        std::cout << "clGetPlatformIDs count failed" << std::endl;
        return std::vector<CLPlatform>();
    }
    std::vector<cl_platform_id> platformIDs(platformCount);
    error = clGetPlatformIDs(platformCount, platformIDs.data(), NULL);
    if (error != CL_SUCCESS)
    {
        std::cout << "clGetPlatformIDs values failed" << std::endl;
        return std::vector<CLPlatform>();
    }
    std::vector<CLPlatform> platforms;
    for (int index = 0; index < platformCount; ++index)
    {
        platforms.push_back(CLPlatform(platformIDs[index]));
    }
    return platforms;
}

std::string CLPlatform::info(cl_platform_info name) const
{
    size_t valueLength;
    cl_int error = clGetPlatformInfo(m_platform_id, name, 0, NULL, &valueLength);
    if (error != CL_SUCCESS)
    {
        std::cout << "clGetPlatformInfo length failed" << std::endl;
        return "";
    }
    std::vector<char> valueBuffer(valueLength);
    error = clGetPlatformInfo(m_platform_id, name, valueLength, valueBuffer.data(), NULL);
    if (error != CL_SUCCESS)
    {
        std::cout << "clGetPlatformInfo value failed" << std::endl;
        return "";
    }
    return valueBuffer.data();
}
