#ifndef HIRTBOXSHADER_H
#define HIRTBOXSHADER_H

#include <crispsynth/OpenGL.h>

class CapsuleShader {
  public:
    static GLuint program;
    static void init();
    static GLuint VAO;

    static GLint uniAspect;
    static GLint uniCapsuleColor;
    static GLint uniRadius;
    static GLint uniHeight;
    static GLint uniViewMatrix;
    static GLint uniInvViewMatrix;
    static GLint uniInvModelMatrix;
    static GLint uniProjectionMatrix;
    static GLint uniFov;
    static GLint uniResolution;
};

#endif // HIRTBOXSHADER_H
