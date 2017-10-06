#include <crispsynth/Locator.h>
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include "CrispPainView.h"

CrispPainView::CrispPainView(QWidget *parent, const QPoint& position, const QSize& size) :
    QSFMLCanvas(parent, position, size) {
        pain = std::make_unique<Pain>(*this);
        font = Locator::getResource()->loadFont("Boogaloo-Regular.ttf");
        text = std::make_unique<TextObject>("test", font);
    }

void CrispPainView::onInit() {
}

void CrispPainView::onUpdate() {
    clear(sf::Color::Black);
    text->render(*pain, 0);
}
