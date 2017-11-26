#ifndef MainMenu_H
#define MainMenu_H

#include <QMainWindow>
#include <QApplication>
#include "CrispPainView.h"
#include <QTableWidget>

class MainMenu : public QMainWindow {
    Q_OBJECT
public:
    explicit MainMenu(QWidget *parent = nullptr);
    QMenu *menuBar;
    CrispPainView *cpview;
    QTableWidget *hurtboxTable;
public slots:
    void open();
    void addHurtbox();
};

#endif
