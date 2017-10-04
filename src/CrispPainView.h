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

private:

    void onInit() override;
    void onUpdate() override;
};

#endif // CRISPPAINVIEW_H
