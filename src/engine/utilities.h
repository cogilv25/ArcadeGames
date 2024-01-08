#ifndef AG_UTILITIES_H
#define AG_UTILITIES_H

#ifdef _DEBUG
#include "stdio.h"
#define GL(x)                                                                              \
    x;                                                                                     \
    {                                                                                      \
        int glError = glGetError();                                                        \
        if (glError != GL_NO_ERROR)                                                        \
        {                                                                                  \
            fprintf(stderr, "OpenGL Error: %d near '%s:%d'\n", glError, __FILE__, __LINE__); \
        }                                                                                  \
    }
#else
#define GL(x) x
#endif

#ifdef _DEBUG

#define IFDBG(x) x

#else

#define IFDBG(x)

#endif

#endif