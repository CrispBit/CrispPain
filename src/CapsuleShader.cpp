#include <crispsynth/OpenGL.h>
#include "CapsuleShader.h"
#include <iostream>

GLuint CapsuleShader::program;
GLuint CapsuleShader::VAO;

GLint CapsuleShader::uniAspect;
GLint CapsuleShader::uniCapsuleColor;
GLint CapsuleShader::uniRadius;
GLint CapsuleShader::uniHeight;
GLint CapsuleShader::uniViewMatrix;
GLint CapsuleShader::uniInvViewMatrix;
GLint CapsuleShader::uniInvModelMatrix;
GLint CapsuleShader::uniProjectionMatrix;
GLint CapsuleShader::uniFov;
GLint CapsuleShader::uniResolution;

const char *vecshader =
"#version 130\n"
"in vec2 pos;\n"
"void main() {\n"
"    gl_Position = vec4(pos, -1.0, 1.0);\n"
"}";

const char *fancyshader =
"#version 130\n"
"\n"
"uniform float aspect;\n"
"uniform vec4 capsuleColor;\n"
"uniform float radius;\n"
"uniform float height;\n"
"uniform mat4 viewMatrix;\n"
"uniform mat4 invViewMatrix;\n"
"uniform mat4 invModelMatrix;\n"
"uniform mat4 projectionMatrix;\n"
"uniform float fov;\n"
"uniform vec2 resolution;\n"
//"uniform vec4 ambient;\n"
"\n"
"const float PI = 3.141592653589793238;\n"
//"vec3 da_sun = vec3(-1.0, -1.0, 0.6);\n"
"\n"
"struct Ray {\n"
"    vec3 pos;\n"
"    vec3 dir;\n"
"};\n"
"\n"
//"vec4 lighting(in vec4 color, in vec3 normal) {\n"
//"    float diffuse = max(dot(-da_sun, normal), 0.0);\n"
//"    return color * (ambient + diffuse);\n"
//"}\n"
"\n"
//"bool sphere(in Ray ray, in float r, inout float d, inout vec3 normal) {\n"
"bool sphere(in Ray ray, in float r, inout float d) {\n"
"    float a = dot(ray.dir, ray.dir);\n"
"    float b = dot(ray.dir, ray.pos);\n"
"    float c = dot(ray.pos, ray.pos) - r*r;\n"
"    float det = b*b - a*c;\n"
"    if (det < 0.0) return false;\n"
"    det = sqrt(det);\n"
"    float d1 = (-b + det) / a;\n"
"    float d2 = (-b - det) / a;\n"
"    d = min(d1, d2);\n"
"    if (d < 0.0) return false;\n"
//"    normal = normalize(ray.pos + ray.dir * d);\n"
"    return true;\n"
"}\n"
"\n"
"// cylinder of length h along the z axis\n"
//"bool cylinder(in Ray ray, in float r, in float h, inout float d, inout vec3 normal) {\n"
"bool cylinder(in Ray ray, in float r, in float h, inout float d) {\n"
"    vec2 xyraydir = ray.dir.xy;\n"
"    vec2 xyraypos = ray.pos.xy;\n"
"    float a = dot(xyraydir, xyraydir);\n"
"    float b = dot(xyraydir, xyraypos);\n"
"    float c = dot(xyraypos, xyraypos) - r*r;\n"
"    float det = b*b - a*c;\n"
"    if (det < 0.0) return false;\n"
"    det = sqrt(det);\n"
"    float d1 = (-b + det) / a;\n"
"    float d2 = (-b - det) / a;\n"
"    d = min(d1, d2);\n"
"    if (d < 0.0) return false;\n"
"    vec3 intersect = ray.pos + ray.dir * d;\n"
"    const vec3 z = vec3(0.0, 0.0, 1.0);\n"
"    float a1 = dot(intersect, z);\n"
"    if (abs(a1) > h/2.0) return false;\n"
//"    normal = normalize(intersect - dot(intersect, z) * z);\n"
"    return true;\n"
"}\n"
"\n"
"void capsule(in Ray ray, in float r, in float h, in mat4 invmat, in vec4 color,\n"
"             inout vec4 fragColor, inout float depth) {\n"
"\n"
"    Ray newray = Ray(vec3(invmat * vec4(ray.pos, 1.0)), mat3(invmat) * ray.dir);\n"
"    float d = depth, ds1, ds2;\n"
//"    vec3 n, ns1, ns2;\n"
//"    bool c = cylinder(newray, r, h, d, n);\n"
"    bool c = cylinder(newray, r, h, d);\n"
"    newray.pos.z += h/2.0;\n"
//"    bool s1 = sphere(newray, r, ds1, ns1);\n"
"    bool s1 = sphere(newray, r, ds1);\n"
"    if (s1 && (ds1 < d || !c)) {\n"
"        d = ds1;\n"
//"        n = ns1;\n"
"    }\n"
"    newray.pos.z -= h;\n"
//"    bool s2 = sphere(newray, r, ds2, ns2);\n"
"    bool s2 = sphere(newray, r, ds2);\n"
"    if (s2 && (ds2 < d || (!c && !s1))) {\n"
"        d = ds2;\n"
//"        n = ns2;\n"
"    }\n"
//"    n = normalize(vec3(transpose(invmat) * vec4(n, 0.0)));\n"
"    if ((c || s1 || s2) && d < depth) {\n"
//"        fragColor = lighting(color, n);\n"
"        fragColor = color;\n"
"        depth = d;\n"
"        vec4 clip = projectionMatrix * viewMatrix * vec4(ray.pos + ray.dir * d, 1.0);\n"
"        float ndc_depth = clip.z / clip.w;\n"
"        gl_FragDepth = ndc_depth / 2.0 + 0.5;\n"
"    } else {\n"
"    }\n"
"}\n"
"\n"
"vec4 calculateColor(Ray ray) {\n"
"    float depth = 999.0;\n"
"    vec4 currentColor = vec4(0.0, 0.0, 0.0, 0.0);\n"
"\n"
"    capsule(ray, radius, height, invModelMatrix, capsuleColor, currentColor, depth);\n"
"\n"
"    return currentColor;\n"
"}\n"
"\n"
"void main()\n"
"{\n"
//"    da_sun = normalize(da_sun);\n"
"    gl_FragDepth = 1.0;\n"
"    vec2 xy = gl_FragCoord.xy / resolution * 2.0 - 1.0;\n"
"    xy.x *= aspect;\n"
"    float d = (aspect/2.0)/tan(radians(fov/2.0));\n"
"    vec3 raydir = mat3(invViewMatrix) * vec3(xy, -d);\n"
"    Ray ray = Ray(invViewMatrix[3].xyz, normalize(raydir));\n"
"    gl_FragColor = calculateColor(ray);\n"
"}";

GLuint compileShader(GLenum type, const char *source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "rip shader comperino " << type << std::endl << infoLog << std::endl;
    }
    return shader;
}

void CapsuleShader::init() {
    GLuint vtx = compileShader(GL_VERTEX_SHADER, vecshader);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fancyshader);
    program = glCreateProgram();

    glAttachShader(program, vtx);
    glAttachShader(program, frag);

    glBindAttribLocation(program, 0, "pos");

    glLinkProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "mfw linking failed\n" << infoLog << std::endl;
    }

    glDeleteShader(vtx);
    glDeleteShader(frag);

    glUseProgram(program);

    GLfloat vertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,

         -1.0f, -1.0f,
          1.0f,  1.0f,
         -1.0f,  1.0f
    };

    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

      GLuint vertex_buffer;
      glGenBuffers(1, &vertex_buffer);
      glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, 0, (const GLvoid*) 0);

    glBindVertexArray(0);

    uniAspect = glGetUniformLocation(program, "aspect");
    uniCapsuleColor = glGetUniformLocation(program, "capsuleColor");
    uniRadius = glGetUniformLocation(program, "radius");
    uniHeight = glGetUniformLocation(program, "height");
    uniViewMatrix = glGetUniformLocation(program, "viewMatrix");
    uniInvViewMatrix = glGetUniformLocation(program, "invViewMatrix");
    uniInvModelMatrix = glGetUniformLocation(program, "invModelMatrix");
    uniProjectionMatrix = glGetUniformLocation(program, "projectionMatrix");
    uniFov = glGetUniformLocation(program, "fov");
    uniResolution = glGetUniformLocation(program, "resolution");
}
