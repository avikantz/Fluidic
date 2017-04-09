#include <iostream>
#include "CLDevice.h"

CLDevice::CLDevice(cl_device_id device_id) : m_device_id(device_id)
{
}

CLDevice::CLDevice(const CLDevice& device) : m_device_id(device.m_device_id)
{
}

CLDevice::~CLDevice()
{
}

std::vector<CLDevice> CLDevice::list(const CLPlatform& platform, cl_device_type type)
{
    cl_uint deviceCount = 0;
    cl_int error = clGetDeviceIDs(platform.m_platform_id, type, 0, NULL, &deviceCount);
    if (error != CL_SUCCESS)
    {
        std::cout << "clGetDeviceIDs count failed" << std::endl;
        return std::vector<CLDevice>();
    }
    std::vector<cl_device_id> deviceIDs(deviceCount);
    error = clGetDeviceIDs(platform.m_platform_id, type, deviceCount, deviceIDs.data(), NULL);
    if (error != CL_SUCCESS)
    {
        std::cout << "clGetDeviceIDs values failed" << std::endl;
        return std::vector<CLDevice>();
    }
    std::vector<CLDevice> devices;
    for (int index = 0; index < deviceCount; ++index)
    {
        devices.push_back(CLDevice(deviceIDs[index]));
    }
    return devices;
}

std::string CLDevice::info(cl_device_info name) const
{
    size_t valueLength;
    cl_int error = clGetDeviceInfo(m_device_id, name, 0, NULL, &valueLength);
    if (error != CL_SUCCESS)
    {
        std::cout << "clGetDeviceInfo length failed" << std::endl;
        return "";
    }
    std::vector<char> valueBuffer(valueLength);
    error = clGetDeviceInfo(m_device_id, name, valueLength, valueBuffer.data(), NULL);
    std::string value(valueBuffer.data());
    if (error != CL_SUCCESS)
    {
        std::cout << "clGetDeviceInfo value failed" << std::endl;
        return "";
    }
    return value;
}
