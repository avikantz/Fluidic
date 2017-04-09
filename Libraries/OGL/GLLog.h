#ifndef GLLOG_H__
#define GLLOG_H__

#include <stdio.h>

#define GL_LOG(...) printf(__VA_ARGS__)

#define GL_ASSERT(glFunc) glFunc; \
	{ \
		GLenum glError = glGetError(); \
		while (glError != GL_NO_ERROR) { \
            printf(__FILE__ " line %d : " #glFunc " ==> 0x%08x\n", __LINE__, glError); \
			glError = glGetError(); \
		} \
	}

#endif
