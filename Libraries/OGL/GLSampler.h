#ifndef GLSAMPLER_H__
#define GLSAMPLER_H__

#include <OpenGL/gl3.h>

class GLSampler
{

public:
	GLSampler();
	virtual ~GLSampler();

	void setParameter(GLenum, GLint);
	void setParameter(GLenum, GLfloat);
	void bind(GLuint target);
	void unbind(GLuint target);

private:
	GLuint m_sampler;

};

#endif
