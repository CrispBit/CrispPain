#ifndef QSFMLCANVAS_H
#define QSFMLCANVAS_H

#include <SFML/Graphics.hpp>
#include <QWidget>

class QSFMLCanvas : public QWidget, public sf::RenderWindow {
    Q_OBJECT

public:
    explicit QSFMLCanvas(QWidget *parent);
    virtual ~QSFMLCanvas();

private:
    virtual void OnInit() = 0;
    virtual void OnUpdate() = 0;
    virtual void showEvent(QShowEvent *);
    virtual QPaintEngine *paintEngine() const;

    bool myInitialized;
};

#endif // QSFMLCANVAS_H
