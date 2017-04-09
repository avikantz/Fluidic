#include <iostream>
#include "CLCommandQueue.h"

CLCommandQueue::CLCommandQueue(const CLContext& context, const CLDevice& device,
                               cl_command_queue_properties properties)
{
    cl_int error;
    m_command_queue = clCreateCommandQueue(context.m_context, device.m_device_id, properties, &error);
    if (error != CL_SUCCESS)
    {
        std::cout << "clCreateCommandQueue failed" << std::endl;
    }
}

CLCommandQueue::~CLCommandQueue()
{
    clFinish(m_command_queue);
    clReleaseCommandQueue(m_command_queue);
}

void CLCommandQueue::finish() const
{
    clFinish(m_command_queue);
}

void CLCommandQueue::flush() const
{
    clFlush(m_command_queue);
}

void CLCommandQueue::enqueueReadBuffer(const CLBuffer &buffer, size_t dataSz, void *data) const
{
    clEnqueueReadBuffer(m_command_queue, buffer.m_buffer, CL_TRUE, 0, dataSz, data, 0, NULL, NULL);
}

void CLCommandQueue::enqueueNDRangeKernel(const CLKernel& kernel,
                                          cl_uint dimensions,
                                          const size_t* globalOffset,
                                          const size_t* globalSize,
                                          const size_t* localSize) const
{
    clEnqueueNDRangeKernel(m_command_queue, kernel.m_kernel,
                           dimensions, globalOffset, globalSize, localSize, 0, nullptr, nullptr);
}

void CLCommandQueue::enqueueAcquireGLBuffer(const CLBufferShared& buffer) const
{
    clEnqueueAcquireGLObjects(m_command_queue, 1, &buffer.m_buffer, 0, nullptr, nullptr);
}

void CLCommandQueue::enqueueReleaseGLBuffer(const CLBufferShared& buffer) const
{
    clEnqueueReleaseGLObjects(m_command_queue, 1, &buffer.m_buffer, 0, nullptr, nullptr);
}
