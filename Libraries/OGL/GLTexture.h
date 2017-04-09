#ifndef GLTEXTURE_H__
#define GLTEXTURE_H__

#include <OpenGL/gl3.h>

class GLTexture
{
    
public:
    GLTexture();
    virtual ~GLTexture();
    
    void bind(GLenum target);
    void unbind(GLenum target);
    
    GLuint name();
    
    void texImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height,
                    GLint border, GLenum format, GLenum type, const GLvoid *pixels);
    
private:
    GLuint m_texture;
    
};

#endif
