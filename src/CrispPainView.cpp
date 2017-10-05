#include <SFML/Graphics.hpp>
#include <cstdlib>
#include "CrispPainView.h"

void CrispPainView::onInit() {
    // do stuffs;
}

void CrispPainView::onUpdate() {
    clear(color);
}

void CrispPainView::randomColor() {
    color = sf::Color(rand());
}
