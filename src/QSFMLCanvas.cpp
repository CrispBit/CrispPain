/*
 * Taken from https://www.sfml-dev.org/tutorials/1.6/graphics-qt.php
 */
#include <SFML/Graphics.hpp>
#include <QWidget>
#include <QTimer>
#include "QSFMLCanvas.h"

#ifdef Q_WS_X11
#include <Qt/qx11info_x11.h>
#include <X11/Xlib.h>
#endif //Q_WS_ZX11

QSFMLCanvas::QSFMLCanvas(QWidget *parent, const QPoint &position, const QSize& size,
        unsigned int frameTime) : QWidget(parent), myInitialized(false) {

    // Setup some states to allow direct rendering into the widget
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    // Set strong focus to enable keyboard events to be received
    setFocusPolicy(Qt::StrongFocus);

    // Setup the widget geometry
    move(position);
    resize(size);

    // Setup the timer
    myTimer.setInterval(frameTime);
}

QSFMLCanvas::~QSFMLCanvas() {}

void QSFMLCanvas::showEvent(QShowEvent*) {
    if (!myInitialized) {
#ifdef Q_WS_X11
        XFlush(QX11Info::display());
#endif

        // Create the SFML window with the widget handle
        sf::RenderWindow::create((sf::WindowHandle) winId());

        // Let the derived class do its specific stuff
        OnInit();

        // Setup the timer to trigger a refresh at specified framerate
        connect(&myTimer, SIGNAL(timeout()), this, SLOT(repaint()));
        myTimer.start();

        myInitialized = true;
    }
}

QPaintEngine *QSFMLCanvas::paintEngine() const {
    return 0;
}

void QSFMLCanvas::paintEvent(QPaintEvent*) {

    // Let the derived class do its specific stuff
    OnUpdate();

    // Display on screen
    display();
}
