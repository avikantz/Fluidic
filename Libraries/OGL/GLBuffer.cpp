#include "GLBuffer.h"
#include "GLLog.h"

GLBuffer::GLBuffer(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
    m_buffer = 0;
    m_target = 0;
    GL_ASSERT(glGenBuffers(1, &m_buffer));
    if (m_buffer)
    {
        m_target = target;
        GL_ASSERT(glBindBuffer(m_target, m_buffer));
        GL_ASSERT(glBufferData(m_target, size, data, usage));
        GL_ASSERT(glBindBuffer(m_target, 0));
    }
}

GLBuffer::~GLBuffer()
{
    GL_ASSERT(glDeleteBuffers(1, &m_buffer));
    m_buffer = 0;
}

GLuint GLBuffer::name()
{
    return m_buffer;
}

void GLBuffer::bind()
{
    GL_ASSERT(glBindBuffer(m_target, m_buffer));
}

void GLBuffer::unbind()
{
    GL_ASSERT(glBindBuffer(m_target, 0));
}
