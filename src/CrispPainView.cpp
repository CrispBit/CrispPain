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

#include "CrispPainView.h"

CrispPainView::CrispPainView(QWidget *parent, const QPoint& position, const QSize& size) :
    QSFMLCanvas(parent, position, size) {
        pain = std::make_unique<Pain>(*this);
        font = Locator::getResource()->loadFont("Boogaloo-Regular.ttf");
        text = std::make_unique<TextObject>("test", font);
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
    MeshShaders::currentProgram = &MeshShaders::bonedMeshShaderProgram;
    glUseProgram(*MeshShaders::currentProgram);

    t_start = std::chrono::high_resolution_clock::now();

    object = meshes.createBoned("bob", "boblampclean.md5mesh");

    uniTrans = glGetUniformLocation(*MeshShaders::currentProgram, "model");
    t_now = std::chrono::high_resolution_clock::now();

    view = glm::lookAt(
            glm::vec3(4.0f, 4.0f, 4.0f),
            glm::vec3(3.0f, 3.0f, 3.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
    );
    view *= glm::scale(glm::vec3(.06f, .06f, .06f));
    uniView = glGetUniformLocation(*MeshShaders::currentProgram, "view");
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

    proj = glm::perspective(glm::radians(lookDeg), 800.0f / 500.0f, 1.0f, 10.0f);
    uniProj = glGetUniformLocation(*MeshShaders::currentProgram, "proj");
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

    glClearColor(0.3f, 0.5f, 0.8f, 1.0f);
    t_now = std::chrono::high_resolution_clock::now();
}


void CrispPainView::onUpdate() {
    t_start = t_now;
    t_now = std::chrono::high_resolution_clock::now();
    glViewport(0, 0, getSize().x, getSize().y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    trans = glm::rotate(
            trans,
            //time * glm::radians(2.0f),
            glm::radians(.1f),
            glm::vec3(0.0f, 1.0f, 0.0f)
    );
    glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));

    std::vector<glm::mat4> Transforms;
    object->boneTransform(clock.getElapsedTime().asSeconds(), Transforms);
    for (unsigned int i = 0; i < Transforms.size(); ++i) {
        const std::string name = "gBones[" + std::to_string(i) + "]"; // every transform is for a different bone
        GLint boneTransform = glGetUniformLocation(MeshShaders::bonedMeshShaderProgram, name.c_str());
        Transforms[i] = glm::transpose(Transforms[i]);
        glUniformMatrix4fv(boneTransform, 1, GL_TRUE, glm::value_ptr(Transforms[i]));
    }

    object->draw();

    //sf::RenderWindow::pushGLStates();
    //text->render(*pain, 0);
    //sf::RenderWindow::popGLStates();
}
