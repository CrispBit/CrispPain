#ifndef CRISPPAINVIEW_H
#define CRISPPAINVIEW_H

#include <SFML/Graphics.hpp>
#include <QWidget>
#include <crispsynth/game-objects/TextObject.h>
#include "QSFMLCanvas.h"
#include "Pain.h"

class CrispPainView : public QSFMLCanvas {

public:

    CrispPainView(QWidget *parent, const QPoint& position, const QSize& size);

private:

    void onInit() override;
    void onUpdate() override;
    sf::Color color = sf::Color(255, 0, 0, 255);
    std::unique_ptr<Pain> pain;
    std::unique_ptr<TextObject> text;
    sf::Font font;
};

#endif // CRISPPAINVIEW_H
