#ifndef QSFMLCANVAS_H
#define QSFMLCANVAS_H

#include <SFML/Graphics.hpp>
#include <QWidget>
#include <QTimer>
#include <QWindow>

class QSFMLCanvas : public QWindow, public sf::RenderWindow {
    Q_OBJECT

public:
    QSFMLCanvas(QWidget *parent, const QPoint &position, const QSize& size, unsigned int frameTime = 5);
    virtual ~QSFMLCanvas();

    QWidget *qwidget;

private:
    virtual void onInit() = 0;
    virtual void onUpdate() = 0;
    virtual QPaintEngine *paintEngine() const;
    virtual void showEvent(QShowEvent *);
    virtual void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent *event);

    QTimer myTimer;
    bool myInitialized;

public slots:
    void repaint();

};

#endif // QSFMLCANVAS_H
