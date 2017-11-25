#ifndef MainMenu_H
#define MainMenu_H

#include <QMainWindow>
#include <QApplication>
#include "CrispPainView.h"

class MainMenu : public QMainWindow {
    Q_OBJECT
public:
    explicit MainMenu(QWidget *parent = nullptr);
    QMenu *menuBar;
    CrispPainView *cpview;
public slots:
    void open();
    void addHurtbox();
};

#endif
