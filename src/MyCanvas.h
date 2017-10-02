#include <SFML/Graphics.hpp>
#include <QWidget>
#include "QSFMLCanvas.h"

class something_Sane : public QSFMLCanvas {

public:

    something_Sane(QWidget *parent, const QPoint& position, const QSize& size) :
        QSFMLCanvas(parent, position, size) {
        // construct the thingo
    }

private:

    void onInit() override;
    void onUpdate() override;
};
