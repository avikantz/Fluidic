#include <vector>

#include "GLProgram.h"
#include "GLLog.h"

GLProgram::GLProgram(const GLchar* vertexSource, const GLchar* fragmentSource)
{
	m_program = 0;
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
	m_program = GL_ASSERT(glCreateProgram());
	if (m_program && vertexShader && fragmentShader)
    {
        GLint linked = GL_FALSE;
        GL_ASSERT(glAttachShader(m_program, vertexShader));
        GL_ASSERT(glAttachShader(m_program, fragmentShader));
        GL_ASSERT(glLinkProgram(m_program));
        GL_ASSERT(glGetProgramiv(m_program, GL_LINK_STATUS, &linked));
        if (!linked)
        {
            GLint infoLogLen = 0;
            GL_ASSERT(glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLogLen));
            GL_LOG("Could not link program");
            if (infoLogLen)
            {
                std::vector<GLchar> infoLog(infoLogLen);
                glGetProgramInfoLog(m_program, infoLogLen, NULL, infoLog.data());
                GL_LOG("Could not link program:\n%s\n", infoLog.data());
            }
            GL_ASSERT(glDeleteProgram(m_program));
            m_program = 0;
        }
	}
    if (vertexShader)
    {
        GL_ASSERT(glDeleteShader(vertexShader));
    }
    if (fragmentShader)
    {
        GL_ASSERT(glDeleteShader(fragmentShader));
    }
}

GLProgram::~GLProgram()
{
	if (m_program)
    {
		GL_ASSERT(glDeleteProgram(m_program));
		m_program = 0;
	}
}

void GLProgram::use()
{
	GL_ASSERT(glUseProgram(m_program));
}

GLint GLProgram::uniform(const GLchar* name)
{
	return GL_ASSERT(glGetUniformLocation(m_program, name));
}

GLuint GLProgram::compileShader(GLenum shaderType, const GLchar* source)
{
	GLuint shader = GL_ASSERT(glCreateShader(shaderType));
	if (shader)
    {
        GLint compiled = GL_FALSE;
		GL_ASSERT(glShaderSource(shader, 1, &source, NULL));
		GL_ASSERT(glCompileShader(shader));
		GL_ASSERT(glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled));
		if (!compiled)
        {
			GLint infoLogLen = 0;
			GL_ASSERT(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen));
			if (infoLogLen > 0)
            {
                std::vector<GLchar> infoLog(infoLogLen);
                GL_ASSERT(glGetShaderInfoLog(shader, infoLogLen, NULL, infoLog.data()));
                GL_LOG("Could not compile %s shader:\n%s\n",
                       shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment",
                       infoLog.data());
			}
			GL_ASSERT(glDeleteShader(shader));
			shader = 0;
		}
	}
	return shader;
}
