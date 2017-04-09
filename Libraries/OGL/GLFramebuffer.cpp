#include "GLFramebuffer.h"
#include "GLLog.h"

GLFramebuffer::GLFramebuffer() {
	m_fbo = 0;
	GL_ASSERT(glGenFramebuffers(1, &m_fbo));
}

GLFramebuffer::~GLFramebuffer() {
	GL_ASSERT(glDeleteFramebuffers(1, &m_fbo));
	m_fbo = 0;
}

void GLFramebuffer::bind(GLenum target) {
	GL_ASSERT(glBindFramebuffer(target, m_fbo));
}

void GLFramebuffer::unbind(GLenum target) {
	GL_ASSERT(glBindFramebuffer(target, 0));
}

void GLFramebuffer::setTexture2D(GLenum attachment, GLuint texture) {
	GL_ASSERT(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, 0));
}

void GLFramebuffer::setDrawBuffers(const GLsizei count, const GLenum* buffers) {
	GL_ASSERT(glDrawBuffers(count, buffers));
}

void GLFramebuffer::checkStatus() {
	GLenum status = GL_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER));
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		GL_LOG("FBO incomplete %d", status);
	}
}
