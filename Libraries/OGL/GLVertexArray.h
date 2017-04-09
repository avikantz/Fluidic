#ifndef GLVERTEXARRAY_H__
#define GLVERTEXARRAY_H__

#include <OpenGL/gl3.h>

class GLVertexArray
{
    
public:
    GLVertexArray();
    virtual ~GLVertexArray();
    
    void bind();
    void unbind();
    
private:
    GLuint m_vao;
    
};

#endif
