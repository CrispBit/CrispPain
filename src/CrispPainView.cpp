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

void CrispPainView::updateHitboxTable(QTableWidget *table) {
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
        hurtboxRotationXItems.at(i)->setValue(hurtbox.rotationX);
        hurtboxRotationYItems.at(i)->setValue(hurtbox.rotationY);
        hurtboxRotationZItems.at(i)->setValue(hurtbox.rotationZ);
    }
}

void CrispPainView::updateHurtboxName(HurtboxComponent &hurtbox, const QString &result) {
    hurtbox.name = result.toStdString();
}

void CrispPainView::updateHurtboxId(HurtboxComponent &hurtbox, int result) {
    hurtbox.id = result;
}

void CrispPainView::updateHurtboxRadius(HurtboxComponent &hurtbox, float result) {
    hurtbox.r = result;
}

void CrispPainView::updateHurtboxHeight(HurtboxComponent &hurtbox, float result) {
    hurtbox.h = result;
}

void CrispPainView::updateHurtboxX(HurtboxComponent &hurtbox, float result) {
    hurtbox.x = result;
}

void CrispPainView::updateHurtboxY(HurtboxComponent &hurtbox, float result) {
    hurtbox.y = result;
}

void CrispPainView::updateHurtboxZ(HurtboxComponent &hurtbox, float result) {
    hurtbox.r = result;
}

void CrispPainView::updateHurtboxSX(HurtboxComponent &hurtbox, float result) {
    hurtbox.sX = result;
}

void CrispPainView::updateHurtboxSY(HurtboxComponent &hurtbox, float result) {
    hurtbox.sY = result;
}

void CrispPainView::updateHurtboxSZ(HurtboxComponent &hurtbox, float result) {
    hurtbox.sZ = result;
}

void CrispPainView::updateHurtboxRotationX(HurtboxComponent &hurtbox, float result) {
    hurtbox.rotationX = result;
}

void CrispPainView::updateHurtboxRotationY(HurtboxComponent &hurtbox, float result) {
    hurtbox.rotationY = result;
}

void CrispPainView::updateHurtboxRotationZ(HurtboxComponent &hurtbox, float result) {
    hurtbox.rotationZ = result;
}

void CrispPainView::addHurtboxTableRow(QTableWidget *table, HurtboxComponent &hurtbox, unsigned int i) {
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
        QDoubleSpinBox *hurtboxRotationXItem = new QDoubleSpinBox();
        QDoubleSpinBox *hurtboxRotationYItem = new QDoubleSpinBox();
        QDoubleSpinBox *hurtboxRotationZItem = new QDoubleSpinBox();

        hurtboxIdItem->setRange(-10000,10000);
        hurtboxRadiusItem->setRange(-10000,10000);
        hurtboxHeightItem->setRange(-10000,10000);
        hurtboxXItem->setRange(-10000,10000);
        hurtboxYItem->setRange(-10000,10000);
        hurtboxZItem->setRange(-10000,10000);
        hurtboxSXItem->setRange(-10000,10000);
        hurtboxSYItem->setRange(-31337,10000);
        hurtboxSZItem->setRange(-10000,10000);
        hurtboxRotationXItem->setRange(-3.2, 3.2);
        hurtboxRotationYItem->setRange(-3.2, 3.2);
        hurtboxRotationZItem->setRange(-3.2, 3.2);
        hurtboxRotationXItem->setSingleStep(.1);
        hurtboxRotationYItem->setSingleStep(.1);
        hurtboxRotationZItem->setSingleStep(.1);

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
        hurtboxRotationXItems.push_back(hurtboxRotationXItem);
        hurtboxRotationYItems.push_back(hurtboxRotationYItem);
        hurtboxRotationZItems.push_back(hurtboxRotationZItem);

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
        table->setCellWidget(i, 10, hurtboxRotationXItem);
        table->setCellWidget(i, 11, hurtboxRotationYItem);
        table->setCellWidget(i, 12, hurtboxRotationZItem);

        QObject::connect(hurtboxIdItem, QOverload<int>::of(&QSpinBox::valueChanged), [&](int result){ updateHurtboxId(hurtbox, result); });
        QObject::connect(hurtboxRadiusItem, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double result){ updateHurtboxRadius(hurtbox, result); });
        QObject::connect(hurtboxHeightItem, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double result){ updateHurtboxHeight(hurtbox, result); });
        QObject::connect(hurtboxXItem, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double result){ updateHurtboxX(hurtbox, result); });
        QObject::connect(hurtboxYItem, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double result){ updateHurtboxY(hurtbox, result); });
        QObject::connect(hurtboxZItem, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double result){ updateHurtboxZ(hurtbox, result); });
        QObject::connect(hurtboxSXItem, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double result){ updateHurtboxSX(hurtbox, result); });
        QObject::connect(hurtboxSYItem, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double result){ updateHurtboxSY(hurtbox, result); });
        QObject::connect(hurtboxSZItem, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double result){ updateHurtboxSZ(hurtbox, result); });
        QObject::connect(hurtboxRotationXItem, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double result){ updateHurtboxRotationX(hurtbox, result); });
        QObject::connect(hurtboxRotationYItem, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double result){ updateHurtboxRotationY(hurtbox, result); });
        QObject::connect(hurtboxRotationZItem, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double result){ updateHurtboxRotationZ(hurtbox, result); });

}

void CrispPainView::createHitboxTable(QTableWidget *table) {

}

void CrispPainView::createHurtboxTable(QTableWidget *table) {
    hurtboxIdItems.clear();
    hurtboxNameItems.clear();
    hurtboxRadiusItems.clear();
    hurtboxRadiusItems.clear();
    hurtboxHeightItems.clear();
    hurtboxXItems.clear();
    hurtboxYItems.clear();
    hurtboxZItems.clear();
    hurtboxSXItems.clear();
    hurtboxSYItems.clear();
    hurtboxSZItems.clear();
    hurtboxRotationXItems.clear();
    hurtboxRotationYItems.clear();
    hurtboxRotationZItems.clear();
    table->setRowCount(boxCollection.hurtboxes.size());
    for (unsigned int i = 0; i < boxCollection.hurtboxes.size(); i++) {
        auto &hurtbox = boxCollection.hurtboxes[i];
        addHurtboxTableRow(table, hurtbox, i);
    }

    updateHurtboxTable(table);
    QObject::connect(table, &QTableWidget::itemChanged, [&](QTableWidgetItem *result) {
        if (result->column() == 1) {
            updateHurtboxName(boxCollection.hurtboxes.at(result->row()), result->text());
        }
    });

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

    object = meshes.createBoned("mfw", "crispbit.fbx");

    uniTrans = glGetUniformLocation(*MeshShaders::currentProgram, "model");

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
    glViewport(0, 0, getSize().x, getSize().y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(*MeshShaders::currentProgram);

    glm::mat4 yabe = glm::scale(glm::vec3(0.1));

    sf::Vector2f resolution(getSize());

    // negate scaling effects caused by window resize
    //setView(sf::View(sf::FloatRect(0, 0, resolution.x, resolution.y)));

    proj = glm::perspective(glm::radians(lookDeg), resolution.x / resolution.y, 10.0f, 300.0f);
    uniProj = glGetUniformLocation(*MeshShaders::currentProgram, "proj");
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

    glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(yabe));

    std::vector<glm::mat4> Transforms;
    float seconds = currentFrame / 60.0f;
    object->boneTransform(seconds, Transforms);
    for (unsigned int i = 0; i < Transforms.size(); ++i) {
        const std::string name = "gBones[" + std::to_string(i) + "]"; // every transform is for a different bone
        GLint boneTransform = glGetUniformLocation(MeshShaders::bonedMeshShaderProgram, name.c_str());
        glUniformMatrix4fv(boneTransform, 1, GL_FALSE, glm::value_ptr(Transforms[i] * object->m_boneInfo[i].boneOffset));
    }

    while (this->currentFrame >= object->m_pScene->mAnimations[1]->mDuration) {
        this->currentFrame -= object->m_pScene->mAnimations[1]->mDuration;
    }

    object->draw();

    glUseProgram(CapsuleShader::program);

    glEnable(GL_SCISSOR_TEST);

    for (auto const &it : boxCollection.hitboxes) {
        auto hitbox = it.second;
        if (!hitbox.hasFrame(this->currentFrame)) {
            continue;
        }
        if (hitbox.currentFrame() != this->currentFrame) {
            hitbox.gotoFrame(this->currentFrame);
        }
        drawCapsule(hitbox.r(), 0, glm::vec4(1.0f, 0.0f, 0.0f, 0.3f), yabe * glm::scale(glm::vec3(20)) * glm::translate(glm::vec3(hitbox.x(), hitbox.y(), hitbox.z())));
    }

    std::vector<sf::RectangleShape> rectangles;
    for (auto hurtbox : boxCollection.hurtboxes) {
        auto thing = Transforms[hurtbox.id];
        glm::mat4 matrix = yabe * thing * glm::scale(glm::vec3(0.01)) *
            glm::mat4(0, 0, 1, 0,
                      1, 0, 0, 0,
                      0, 1, 0, 0,
                      0, 0, 0, 1) * glm::translate(glm::vec3(hurtbox.x, hurtbox.y, hurtbox.z)) * glm::eulerAngleXYZ(hurtbox.rotationX, hurtbox.rotationY, hurtbox.rotationZ);
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

void CrispPainView::createHitbox(QTableWidget *table) {
    boxCollection.hitboxes[0].gotoFrame(this->currentFrame);
}

void CrispPainView::createHurtbox(QTableWidget *table) {
    boxCollection.hurtboxes.push_back(HurtboxComponent());
    unsigned int size = boxCollection.hurtboxes.size();
    table->insertRow(size-1);
    addHurtboxTableRow(table, boxCollection.hurtboxes.back(), size-1);
}

void CrispPainView::nextFrame() {
    this->currentFrame++;
}

void CrispPainView::prevFrame() {
    if (this->currentFrame > 0) {
        this->currentFrame--;
    }
}
