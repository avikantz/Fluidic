#include <iostream>
#include "CLKernel.h"

CLKernel::CLKernel(const CLProgram& program, const char* name)
{
    cl_int error;
    m_kernel = clCreateKernel(program.m_program, name, &error);
    if (error != CL_SUCCESS)
    {
        std::cout << "clCreateKernel " << name << "failed" << std::endl;
    }
}

CLKernel::~CLKernel()
{
    clReleaseKernel(m_kernel);
}

size_t CLKernel::maxWorkGroupSize() const
{
    size_t value = 0;
    clGetKernelWorkGroupInfo(m_kernel, NULL, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &value, NULL);
    return value;
}

size_t CLKernel::preferredWorkGroupSizeMultiple() const
{
    size_t value = 0;
    clGetKernelWorkGroupInfo(m_kernel, NULL, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, sizeof(size_t), &value, NULL);
    return value;
}

void CLKernel::setArg(cl_uint location, const CLBuffer& buffer) const
{
    cl_int error = clSetKernelArg(m_kernel, location, sizeof(cl_mem), &buffer.m_buffer);
    if (error != CL_SUCCESS)
    {
        std::cout << "clSetKernelArg failed" << std::endl;
    }
}

void CLKernel::setArg(cl_uint location, size_t size, const void* data) const
{
    cl_int error = clSetKernelArg(m_kernel, location, size, data);
    if (error != CL_SUCCESS)
    {
        std::cout << "clSetKernelArg failed" << std::endl;
    }
}

void CLKernel::setArg(cl_uint location, const CLBufferShared& buffer) const
{
    cl_int error = clSetKernelArg(m_kernel, location, sizeof(cl_mem), &buffer.m_buffer);
    if (error != CL_SUCCESS)
    {
        std::cout << "clSetKernelArg failed" << std::endl;
    }
}

void CLKernel::setArg(cl_uint location, const CLMem& buffer) const
{
    cl_int error = clSetKernelArg(m_kernel, location, sizeof(cl_mem), &buffer.m_mem);
    if (error != CL_SUCCESS)
    {
        std::cout << "clSetKernelArg failed" << std::endl;
    }
}
