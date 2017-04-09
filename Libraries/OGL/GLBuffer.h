#ifndef GLBUFFER_H__
#define GLBUFFER_H__

#include <OpenGL/gl3.h>

class GLBuffer
{
    
public:
    GLBuffer(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
    virtual ~GLBuffer();
    
    GLuint name();
    void bind();
    void unbind();
    
private:
    GLenum m_target;
    GLuint m_buffer;
    
};

#endif
