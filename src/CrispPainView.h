#ifndef CRISPPAINVIEW_H
#define CRISPPAINVIEW_H

#include <chrono>

#include <SFML/Graphics.hpp>

#include <QWidget>
#include <QTableWidget>
#include <QSpinBox>

#include <crispsynth/game-objects/TextObject.h>
#include <crispsynth/game-objects/BoxCollection.h>
#include <crispsynth/mesh/MeshContainer.h>

#include "QSFMLCanvas.h"
#include "Pain.h"

class CrispPainView : public QSFMLCanvas {

public:

    CrispPainView(QWidget *parent, const QPoint& position, const QSize& size);
    void createHurtboxTable(QTableWidget *table);
    void updateHurtboxTable(QTableWidget *table);
    void createHurtbox(QTableWidget *table);
    BoxCollection boxCollection;

private:

    void onInit() override;
    void onUpdate() override;
    sf::FloatRect capsuleBoundingBox(float radius, float height, glm::mat4 modelMatrix);
    void drawCapsule(float radius, float height, glm::vec4 color, glm::mat4 modelMatrix);
    sf::Color color = sf::Color(255, 0, 0, 255);
    std::unique_ptr<Pain> pain;
    std::unique_ptr<TextObject> text;
    sf::Font font;

    std::chrono::time_point<std::chrono::high_resolution_clock> t_start;
    std::chrono::time_point<std::chrono::high_resolution_clock> t_now;

    BonedMesh* object;
    sf::Clock clock;
    MeshContainer meshes;

    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 trans;

    GLint uniProj;
    GLint uniTrans;
    GLint uniView;

    float lookDeg = 50;

    std::vector<QSpinBox*> hurtboxIdItems;
    std::vector<QTableWidgetItem*> hurtboxNameItems;
    std::vector<QDoubleSpinBox*> hurtboxRadiusItems,
                                 hurtboxHeightItems,
                                 hurtboxXItems,
                                 hurtboxYItems,
                                 hurtboxZItems,
                                 hurtboxSXItems,
                                 hurtboxSYItems,
                                 hurtboxSZItems;
};

#endif // CRISPPAINVIEW_H
