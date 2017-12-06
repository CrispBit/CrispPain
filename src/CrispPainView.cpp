#include <crispsynth/OpenGL.h>

#include <iostream>
#include <chrono>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/matrix.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include <crispsynth/Locator.h>
#include <crispsynth/resources/LocalResources.h>
#include <crispsynth/mesh/MeshContainer.h>
#include <crispsynth/mesh/MeshShaders.h>

#include "CapsuleShader.h"

#include "CrispPainView.h"

#include <QTableWidgetItem>

glm::vec3 eye = glm::vec3(100.0f, 100.0f, 100.0f);
glm::vec3 center = glm::vec3(0.0f, 30.0f, 0.0f);

sf::FloatRect CrispPainView::capsuleBoundingBox(float capRadius, float capHeight, glm::mat4 modelMatrix) {

    glm::vec4 boxVertices[8] = {
        glm::vec4(capRadius, capRadius, capRadius + capHeight/2.0f, 1.0f),
        glm::vec4(-capRadius, capRadius, capRadius + capHeight/2.0f, 1.0f),
        glm::vec4(capRadius, -capRadius, capRadius + capHeight/2.0f, 1.0f),
        glm::vec4(-capRadius, -capRadius, capRadius + capHeight/2.0f, 1.0f),

        glm::vec4(capRadius, capRadius, -capRadius - capHeight/2.0f, 1.0f),
        glm::vec4(-capRadius, capRadius, -capRadius - capHeight/2.0f, 1.0f),
        glm::vec4(capRadius, -capRadius, -capRadius - capHeight/2.0f, 1.0f),
        glm::vec4(-capRadius, -capRadius, -capRadius - capHeight/2.0f, 1.0f)
    };
    float ndc_top = -1.0f,
       ndc_bottom = 1.0f,
        ndc_right = -1.0f,
         ndc_left = 1.0f;

    for (glm::vec4 &vertex : boxVertices) {
        vertex = proj * view * modelMatrix * vertex;
        vertex.x /= vertex.w;
        vertex.y /= vertex.w;
        ndc_top = std::max(ndc_top, vertex.y);
        ndc_bottom = std::min(ndc_bottom, vertex.y);
        ndc_left = std::min(ndc_left, vertex.x);
        ndc_right = std::max(ndc_right, vertex.x);
    }

    sf::Vector2f resolution(getSize());

    float top = (-ndc_top + 1.0f) / 2.0f * resolution.y;
    float left = (ndc_left + 1.0f) / 2.0f * resolution.x;
    float width = (ndc_right - ndc_left) / 2.0f * resolution.x;
    float height = (ndc_top - ndc_bottom) / 2.0f * resolution.y;

    return sf::FloatRect(std::floor(left), std::floor(top), std::ceil(width), std::ceil(height));
}

void CrispPainView::drawCapsule(float radius, float height, glm::vec4 color, glm::mat4 modelMatrix) {

    sf::Vector2f resolution(getSize());

    sf::FloatRect boundingBox = capsuleBoundingBox(radius, height, modelMatrix);
    glScissor(boundingBox.left, resolution.y - boundingBox.top - boundingBox.height, boundingBox.width, boundingBox.height);

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
}

void CrispPainView::updateHurtboxTable(QTableWidget *table) {
    for (unsigned int i = 0; i < boxCollection.hurtboxes.size(); i++) {
        const auto &hurtbox = boxCollection.hurtboxes.at(i);
        hurtboxNameItems.at(i)->setText(QString::fromStdString(hurtbox.name));
        hurtboxIdItems.at(i)->setValue(hurtbox.id);
        hurtboxRadiusItems.at(i)->setValue(hurtbox.r);
        hurtboxHeightItems.at(i)->setValue(hurtbox.h);
        hurtboxXItems.at(i)->setValue(hurtbox.x);
        hurtboxYItems.at(i)->setValue(hurtbox.y);
        hurtboxZItems.at(i)->setValue(hurtbox.z);
        hurtboxSXItems.at(i)->setValue(hurtbox.sX);
        hurtboxSYItems.at(i)->setValue(hurtbox.sY);
        hurtboxSZItems.at(i)->setValue(hurtbox.sZ);
    }
}

void CrispPainView::createHurtboxTable(QTableWidget *table) {
    table->setRowCount(boxCollection.hurtboxes.size());
    for (unsigned int i = 0; i < boxCollection.hurtboxes.size(); i++) {
        const auto &hurtbox = boxCollection.hurtboxes[i];
        std::cout << hurtbox.id << std::endl;
        std::cout << hurtbox.name << std::endl;
        QSpinBox *hurtboxIdItem = new QSpinBox();
        QTableWidgetItem *hurtboxNameItem = new QTableWidgetItem();
        QDoubleSpinBox *hurtboxRadiusItem = new QDoubleSpinBox();
        QDoubleSpinBox *hurtboxHeightItem = new QDoubleSpinBox();
        QDoubleSpinBox *hurtboxXItem = new QDoubleSpinBox();
        QDoubleSpinBox *hurtboxYItem = new QDoubleSpinBox();
        QDoubleSpinBox *hurtboxZItem = new QDoubleSpinBox();
        QDoubleSpinBox *hurtboxSXItem = new QDoubleSpinBox();
        QDoubleSpinBox *hurtboxSYItem = new QDoubleSpinBox();
        QDoubleSpinBox *hurtboxSZItem = new QDoubleSpinBox();

        hurtboxIdItem->setRange(-10000,10000);
        hurtboxRadiusItem->setRange(-10000,10000);
        hurtboxHeightItem->setRange(-10000,10000);
        hurtboxXItem->setRange(-10000,10000);
        hurtboxYItem->setRange(-10000,10000);
        hurtboxZItem->setRange(-10000,10000);
        hurtboxSXItem->setRange(-10000,10000);
        hurtboxSYItem->setRange(-31337,10000);
        hurtboxSZItem->setRange(-10000,10000);

        hurtboxIdItems.push_back(hurtboxIdItem);
        hurtboxNameItems.push_back(hurtboxNameItem);
        hurtboxRadiusItems.push_back(hurtboxRadiusItem);
        hurtboxHeightItems.push_back(hurtboxHeightItem);
        hurtboxXItems.push_back(hurtboxXItem);
        hurtboxYItems.push_back(hurtboxYItem);
        hurtboxZItems.push_back(hurtboxZItem);
        hurtboxSXItems.push_back(hurtboxSXItem);
        hurtboxSYItems.push_back(hurtboxSYItem);
        hurtboxSZItems.push_back(hurtboxSZItem);

        table->setCellWidget(i, 0, hurtboxIdItem);
        table->setItem(i, 1,  hurtboxNameItem);
        table->setCellWidget(i, 2,  hurtboxRadiusItem);
        table->setCellWidget(i, 3,  hurtboxHeightItem);
        table->setCellWidget(i, 4,  hurtboxXItem);
        table->setCellWidget(i, 5,  hurtboxYItem);
        table->setCellWidget(i, 6,  hurtboxZItem);
        table->setCellWidget(i, 7,  hurtboxSXItem);
        table->setCellWidget(i, 8,  hurtboxSYItem);
        table->setCellWidget(i, 9,  hurtboxSZItem);
    }
    updateHurtboxTable(table);
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
}

void CrispPainView::onUpdate() {
    t_start = t_now;
    t_now = std::chrono::high_resolution_clock::now();
    glViewport(0, 0, getSize().x, getSize().y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(*MeshShaders::currentProgram);

    trans = glm::rotate(
            trans,
            glm::radians(.1f),
            glm::vec3(0.0f, 1.0f, 0.0f)
    );

    glm::mat4 yabe = glm::scale(glm::vec3(0.1)) * trans;

    sf::Vector2f resolution(getSize());

    // negate scaling effects caused by window resize
    //setView(sf::View(sf::FloatRect(0, 0, resolution.x, resolution.y)));

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

    glEnable(GL_SCISSOR_TEST);

    std::vector<sf::RectangleShape> rectangles;
    for (auto hurtbox : boxCollection.hurtboxes) {
        auto thing = Transforms[hurtbox.id];
        glm::mat4 matrix = yabe * thing * glm::scale(glm::vec3(0.01)) *
            glm::mat4(0, 0, 1, 0,
                      1, 0, 0, 0,
                      0, 1, 0, 0,
                      0, 0, 0, 1) * glm::translate(glm::vec3(hurtbox.x, hurtbox.y, hurtbox.z));
        drawCapsule(hurtbox.r, 2 * hurtbox.h, glm::vec4(1.0f, 1.0f, 0.0f, 0.5f), matrix);
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

        /* bounding box drawing for testing purposes
        sf::FloatRect boundingBox = capsuleBoundingBox(1.0f, 3.0f, matrix);
        sf::RectangleShape waows;
        waows.setSize(sf::Vector2f(boundingBox.width, boundingBox.height));
        waows.setPosition(boundingBox.left, boundingBox.top);
        waows.setOutlineColor(sf::Color::Red);
        waows.setOutlineThickness(2);
        waows.setFillColor(sf::Color::Transparent);
        rectangles.push_back(waows);*/
    }

    glDisable(GL_SCISSOR_TEST);

    sf::RenderWindow::pushGLStates();
    text->render(*pain, 0);
    /* bounding box drawing for testing purposes
    for (sf::RectangleShape rectangle : rectangles) {
        sf::RenderWindow::draw(rectangle);
    }
    */
    sf::RenderWindow::popGLStates();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void CrispPainView::createHurtbox(QTableWidget *table) {
    std::cout << "lol" << std::endl;
}
