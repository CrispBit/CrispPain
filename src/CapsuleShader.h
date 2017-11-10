#ifndef HIRTBOXSHADER_H
#define HIRTBOXSHADER_H

#include <crispsynth/OpenGL.h>

class CapsuleShader {
  public:
    static GLuint program;
    static void init();
    static GLuint VAO;
};

#endif // HIRTBOXSHADER_H
