#ifndef CRISPPAINVIEW_H
#define CRISPPAINVIEW_H

#include <chrono>
#include <SFML/Graphics.hpp>
#include <QWidget>
#include <crispsynth/game-objects/TextObject.h>
#include <crispsynth/mesh/MeshContainer.h>
#include "QSFMLCanvas.h"
#include "Pain.h"

class CrispPainView : public QSFMLCanvas {

public:

    CrispPainView(QWidget *parent, const QPoint& position, const QSize& size);

private:

    void onInit() override;
    void onUpdate() override;
    void drawCapsule(glm::vec4 color);
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
};

#endif // CRISPPAINVIEW_H
