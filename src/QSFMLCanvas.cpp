#include <SFML/Graphics.hpp>
#include <QWidget>
#include "QSFMLCanvas.h"

#ifdef Q_WS_X11
#include <Qt/qx11info_x11.h>
#include <X11/Xlib.h>
#endif //Q_WS_ZX11

QSFMLCanvas::QSFMLCanvas(QWidget *parent) : QWidget(parent), myInitialized(false) {
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    setFocusPolicy(Qt::StrongFocus);
}

QSFMLCanvas::~QSFMLCanvas() {}

void QSFMLCanvas::showEvent(QShowEvent*) {
    if (!myInitialized) {
#ifdef Q_WS_X11
        XFlush(QX11Info::display());
#endif
    }
}

QPaintEngine *QSFMLCanvas::paintEngine() const {
    return 0;
}
