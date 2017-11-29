/*
 * Taken from https://www.sfml-dev.org/tutorials/1.6/graphics-qt.php
 */
#include <SFML/Graphics.hpp>
#include <QWidget>
#include <QWindow>
#include <QTimer>
#include "QSFMLCanvas.h"

#ifdef Q_WS_X11
#include <Qt/qx11info_x11.h>
#include <X11/Xlib.h>
#endif //Q_WS_ZX11

QSFMLCanvas::QSFMLCanvas(QWidget *parent, const QPoint &position, const QSize& size,
        unsigned int frameTime) : QWindow() {

    setSurfaceType(QWindow::OpenGLSurface);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    setFormat(format);

    qwidget = QWidget::createWindowContainer(this, parent);
    qwidget->move(position);
    qwidget->resize(size);

    myTimer.setInterval(frameTime);
}

QSFMLCanvas::~QSFMLCanvas() {}

void QSFMLCanvas::showEvent(QShowEvent*) {
    if (!myInitialized) {
#ifdef Q_WS_X11
        XFlush(QX11Info::display());
#endif

        // Create the SFML window with the widget handle
        sf::RenderWindow::create((sf::WindowHandle) winId(), sf::ContextSettings(24, 0, 0, 3, 0));

        // Let the derived class do its specific stuff
        onInit();

        // Setup the timer to trigger a refresh at specified framerate
        connect(&myTimer, SIGNAL(timeout()), this, SLOT(repaint()));
        myTimer.start();

        myInitialized = true;
    }
}

QPaintEngine *QSFMLCanvas::paintEngine() const {
    return 0;
}

void QSFMLCanvas::repaint() {
    paintEvent(0); // hehe
}

void QSFMLCanvas::paintEvent(QPaintEvent*) {

    // Let the derived class do its specific stuff
    onUpdate();

    // Display on screen
    display();
}

void QSFMLCanvas::resizeEvent(QResizeEvent* event) {
    setSize(sf::Vector2u(QWindow::width(), QWindow::height()));
}
