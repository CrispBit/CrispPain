#ifndef CRISPPAINVIEW_H
#define CRISPPAINVIEW_H

#include <SFML/Graphics.hpp>
#include <QWidget>
#include "QSFMLCanvas.h"

class CrispPainView : public QSFMLCanvas {

public:

    CrispPainView(QWidget *parent, const QPoint& position, const QSize& size) :
        QSFMLCanvas(parent, position, size) {
        // construct the thingo
    }
    void randomColor();

private:

    void onInit() override;
    void onUpdate() override;
    sf::Color color = sf::Color(255, 0, 0, 255);
};

#endif // CRISPPAINVIEW_H
