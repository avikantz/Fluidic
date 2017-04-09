#ifndef GLPROGRAM_H__
#define GLPROGRAM_H__

#include <OpenGL/gl3.h>

class GLProgram {

public:
	GLProgram(const GLchar* vertexSource, const GLchar* fragmentSource);
	virtual ~GLProgram();

	void use();
	GLint uniform(const GLchar* name);

private:
	GLuint compileShader(GLenum shaderType, const GLchar* source);

private:
	GLuint m_program;

};

#endif
