#ifndef QSFMLCANVAS_H
#define QSFMLCANVAS_H

#include <SFML/Graphics.hpp>
#include <QWidget>
#include <QTimer>

class QSFMLCanvas : public QWidget, public sf::RenderWindow {
    Q_OBJECT

public:
    QSFMLCanvas(QWidget *parent, const QPoint &position, const QSize& size, unsigned int frameTime = 5);
    virtual ~QSFMLCanvas();

private:
    virtual void onInit() = 0;
    virtual void onUpdate() = 0;
    virtual QPaintEngine *paintEngine() const;
    virtual void showEvent(QShowEvent *);
    virtual void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent *event);

    QTimer myTimer;
    bool myInitialized;
};

#endif // QSFMLCANVAS_H
