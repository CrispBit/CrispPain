#include <crispsynth/OpenGL.h>

#include <iostream>
#include <chrono>
#include <cstdlib>

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/matrix.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <crispsynth/Locator.h>
#include <crispsynth/resources/LocalResources.h>
#include <crispsynth/mesh/MeshContainer.h>
#include <crispsynth/mesh/MeshShaders.h>

#include "CapsuleShader.h"

#include "CrispPainView.h"

#include <glm/ext.hpp>

glm::vec3 eye = glm::vec3(100.0f, 100.0f, 100.0f);
glm::vec3 center = glm::vec3(0.0f, 30.0f, 0.0f);

void CrispPainView::drawCapsule(float radius, float height, glm::vec4 color, glm::mat4 modelMatrix) {

    sf::Vector2f resolution(getSize());

    glBindVertexArray(CapsuleShader::VAO);
      glUniform1f(CapsuleShader::uniAspect, resolution.x / resolution.y);
      glUniform1f(CapsuleShader::uniFov, lookDeg);
      glUniform1f(CapsuleShader::uniHeight, height);
      glUniform1f(CapsuleShader::uniRadius, radius);
      glUniform2f(CapsuleShader::uniResolution, resolution.x, resolution.y);
      glUniform4fv(CapsuleShader::uniCapsuleColor, 1, glm::value_ptr(color));

      glUniformMatrix4fv(CapsuleShader::uniInvModelMatrix, 1, GL_FALSE, glm::value_ptr(glm::inverse(modelMatrix)));
      glUniformMatrix4fv(CapsuleShader::uniInvViewMatrix, 1, GL_FALSE, glm::value_ptr(glm::inverse(view)));
      glUniformMatrix4fv(CapsuleShader::uniViewMatrix, 1, GL_FALSE, glm::value_ptr(view));
      glUniformMatrix4fv(CapsuleShader::uniProjectionMatrix, 1, GL_FALSE, glm::value_ptr(proj));

      glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

CrispPainView::CrispPainView(QWidget *parent, const QPoint& position, const QSize& size) :
    QSFMLCanvas(parent, position, size) {
        pain = std::make_unique<Pain>(*this);
        font = Locator::getResource()->loadFont("Boogaloo-Regular.ttf");
        text = std::make_unique<TextObject>("test", font);
        //text->updatePosition(320, 170);
}

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

    MeshShaders::init();
    CapsuleShader::init();
    MeshShaders::currentProgram = &MeshShaders::bonedMeshShaderProgram;
    glUseProgram(*MeshShaders::currentProgram);

    t_start = std::chrono::high_resolution_clock::now();

    object = meshes.createBoned("mfw", "crispbit.fbx");

    uniTrans = glGetUniformLocation(*MeshShaders::currentProgram, "model");
    t_now = std::chrono::high_resolution_clock::now();

    view = glm::lookAt(
            eye,
            center,
            glm::vec3(0.0f, 1.0f, 0.0f)
    );
    uniView = glGetUniformLocation(*MeshShaders::currentProgram, "view");
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

    glClearColor(0.3f, 0.5f, 0.8f, 1.0f);
    /*t_now = std::chrono::high_resolution_clock::now();*/
}


void CrispPainView::onUpdate() {
    t_start = t_now;
    t_now = std::chrono::high_resolution_clock::now();
    glViewport(0, 0, getSize().x, getSize().y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(*MeshShaders::currentProgram);

    trans = glm::rotate(
            trans,
            //time * glm::radians(2.0f),
            glm::radians(.1f),
            glm::vec3(0.0f, 1.0f, 0.0f)
    );

    glm::mat4 yabe = glm::scale(glm::vec3(0.1)) * trans;

    sf::Vector2f resolution(getSize());
    proj = glm::perspective(glm::radians(lookDeg), resolution.x / resolution.y, 10.0f, 300.0f);
    uniProj = glGetUniformLocation(*MeshShaders::currentProgram, "proj");
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

    glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(yabe));

    std::vector<glm::mat4> Transforms;
    float seconds = clock.getElapsedTime().asSeconds();
    object->boneTransform(seconds, Transforms);
    for (unsigned int i = 0; i < Transforms.size(); ++i) {
        const std::string name = "gBones[" + std::to_string(i) + "]"; // every transform is for a different bone
        GLint boneTransform = glGetUniformLocation(MeshShaders::bonedMeshShaderProgram, name.c_str());
        glUniformMatrix4fv(boneTransform, 1, GL_FALSE, glm::value_ptr(Transforms[i] * object->m_boneInfo[i].boneOffset));
    }
    uint16_t frame = seconds * object->m_pScene->mAnimations[1]->mTicksPerSecond;
    while (frame >= object->m_pScene->mAnimations[1]->mDuration) {
        frame -= object->m_pScene->mAnimations[1]->mDuration;
    }

    object->draw();

    glUseProgram(CapsuleShader::program);
    //glClear(GL_DEPTH_BUFFER_BIT);

    switch (frame) {
        case 45:
        case 46:
            drawCapsule(3.0f, 6.0f, glm::vec4(1.0f, 0.0f, 0.0f, 0.3f), yabe * glm::scale(glm::vec3(20)) * glm::rotate(60.0f, glm::vec3(1, 0, 0)) * glm::translate(glm::vec3(3, -10, 10)));
            break;
        case 47:
        case 48:
        case 49:
        case 50:
            drawCapsule(3.0f, 6.0f, glm::vec4(1.0f, 0.0f, 0.0f, 0.3f), yabe * glm::scale(glm::vec3(20)) * glm::rotate(60.0f, glm::vec3(1, 0, 0)) * glm::rotate(-.6f, glm::vec3(0, 1, 0)) * glm::translate(glm::vec3(10, -10, 15)));
            break;
    }

    for (auto thing : Transforms) {
        glm::mat4 matrix = yabe * thing * glm::scale(glm::vec3(0.01)) *
            glm::mat4(0, 0, 1, 0,
                      1, 0, 0, 0,
                      0, 1, 0, 0,
                      0, 0, 0, 1) * glm::translate(glm::vec3(0, 0, 1.5));
        drawCapsule(1.0f, 3.0f, glm::vec4(1.0f, 1.0f, 0.0f, 0.5f), matrix);
        // y axis
        drawCapsule(.1f, 3.0f, glm::vec4(0.0f, 1.0f, 0.0f, 0.3f), matrix * glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f) * glm::translate(glm::vec3(0.0f, 0.0f, 1.5f)));
        // x axis
        drawCapsule(0.1f, 3.0f, glm::vec4(1.0f, 0.0f, 0.0f, 0.3f), matrix * glm::mat4(0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f) * glm::translate(glm::vec3(0.0f, 0.0f, 1.5f)));
    }

    sf::RenderWindow::pushGLStates();
    text->render(*pain, 0);
    sf::RenderWindow::popGLStates();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
