#ifndef GLFRAMEBUFFER_H__
#define GLFRAMEBUFFER_H__

#include <OpenGL/gl3.h>

class GLFramebuffer {

public:
	GLFramebuffer();
	virtual ~GLFramebuffer();

	void bind(GLenum);
	void unbind(GLenum);
	void setTexture2D(GLenum, GLuint);
	void setDrawBuffers(const GLsizei, const GLenum*);
	void checkStatus();

private:
	GLuint m_fbo;

};

#endif
