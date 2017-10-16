#include <iostream>
#include "glad/glad.h"
#include <crispsynth/Locator.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include "CrispPainView.h"

CrispPainView::CrispPainView(QWidget *parent, const QPoint& position, const QSize& size) :
    QSFMLCanvas(parent, position, size) {
        pain = std::make_unique<Pain>(*this);
        font = Locator::getResource()->loadFont("Boogaloo-Regular.ttf");
        text = std::make_unique<TextObject>("test", font);
}

const char *vtxShaderSource = "#version 130\n"
    "in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *frgtShaderSource = "#version 130\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";
void CrispPainView::onInit() {
    if (!gladLoadGL()) {
        std::cerr << "rip" << std::endl;
        exit(2);
    }

    sf::ContextSettings settings = getSettings();
    std::cout << "depth bits:" << settings.depthBits << std::endl;
    std::cout << "stencil bits:" << settings.stencilBits << std::endl;
    std::cout << "antialiasing level:" << settings.antialiasingLevel << std::endl;
    std::cout << "version:" << settings.majorVersion << "." << settings.minorVersion << std::endl;

    GLuint vtxshdr = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vtxshdr, 1, &vtxShaderSource, NULL);
    glCompileShader(vtxshdr);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vtxshdr, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vtxshdr, 512, NULL, infoLog);
        std::cout << "rip vertex shader: " << infoLog << std::endl;
    }

    GLuint frgtshdr = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frgtshdr, 1, &frgtShaderSource, NULL);
    glCompileShader(frgtshdr);

    glGetShaderiv(frgtshdr, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(frgtshdr, 512, NULL, infoLog);
        std::cout << "rip fragment shader: " << infoLog << std::endl;
    }

    GLuint shdrprgm = glCreateProgram();
    glAttachShader(shdrprgm, vtxshdr);
    glAttachShader(shdrprgm, frgtshdr);
    glLinkProgram(shdrprgm);

    glGetShaderiv(shdrprgm, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(frgtshdr, 512, NULL, infoLog);
        std::cout << "mfw linking failed: " << infoLog << std::endl;
    }
    glDeleteShader(vtxshdr);
    glDeleteShader(frgtshdr);

    GLfloat numbers[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    GLuint indices[] = {
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    // mfw comments

    GLuint VBO, EBO; // :)
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(numbers), numbers, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GLint apos = glGetAttribLocation(shdrprgm, "aPos");
    std::cout << apos << std::endl;
    glVertexAttribPointer(apos, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), NULL);
    glEnableVertexAttribArray(apos);
    glUseProgram(shdrprgm);
}

void CrispPainView::onUpdate() {
    glViewport(0, 0, getSize().x, getSize().y);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    //text->render(*pain, 0);
}
