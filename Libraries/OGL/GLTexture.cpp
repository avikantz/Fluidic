#include "GLTexture.h"
#include "GLLog.h"

GLTexture::GLTexture()
{
    m_texture = 0;
    GL_ASSERT(glGenTextures(1, &m_texture));
}

GLTexture::~GLTexture()
{
    GL_ASSERT(glDeleteTextures(1, &m_texture));
    m_texture = 0;
}

void GLTexture::bind(GLenum target)
{
    GL_ASSERT(glBindTexture(target, m_texture));
}

void GLTexture::unbind(GLenum target)
{
    GL_ASSERT(glBindTexture(target, 0));
}

GLuint GLTexture::name()
{
    return m_texture;
}

void GLTexture::texImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height,
                           GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
    GL_ASSERT(glTexImage2D(target, level, internalFormat, width, height, border, format, type, pixels));
}

