#ifndef MainMenu_H
#define MainMenu_H

#include <QMainWindow>
#include <QApplication>

class MainMenu : public QMainWindow {
    Q_OBJECT
public:
    explicit MainMenu(QWidget *parent = nullptr);
public slots:
    void open();
};

#endif
