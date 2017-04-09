#include "GLSampler.h"
#include "GLLog.h"

GLSampler::GLSampler()
{
	m_sampler = 0;
	GL_ASSERT(glGenSamplers(1, &m_sampler));
}

GLSampler::~GLSampler()
{
    GL_ASSERT(glDeleteSamplers(1, &m_sampler));
    m_sampler = 0;
}

void GLSampler::setParameter(GLenum pname, GLint param) {
	GL_ASSERT(glSamplerParameteri(m_sampler, pname, param));
}

void GLSampler::setParameter(GLenum pname, GLfloat param) {
	GL_ASSERT(glSamplerParameterf(m_sampler, pname, param));
}

void GLSampler::bind(GLuint unit) {
	GL_ASSERT(glBindSampler(unit, m_sampler));
}

void GLSampler::unbind(GLuint unit) {
	GL_ASSERT(glBindSampler(unit, 0));
}
