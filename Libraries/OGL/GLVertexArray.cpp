#include "GLVertexArray.h"
#include "GLLog.h"

GLVertexArray::GLVertexArray()
{
    m_vao = 0;
    GL_ASSERT(glGenVertexArrays(1, &m_vao));
}

GLVertexArray::~GLVertexArray()
{
    GL_ASSERT(glDeleteVertexArrays(1, &m_vao));
    m_vao = 0;
}

void GLVertexArray::bind()
{
    GL_ASSERT(glBindVertexArray(m_vao));
}

void GLVertexArray::unbind()
{
    GL_ASSERT(glBindVertexArray(0));
}
