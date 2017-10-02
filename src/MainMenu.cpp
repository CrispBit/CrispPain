#include "MainMenu.h"
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <iostream>

MainMenu::MainMenu(QWidget *parent) : QMainWindow(parent) {
    QPixmap newPix("new.png");
    QPixmap openPix("open.png");
    QPixmap quitPix("quit.png");

    QAction *newA = new QAction(newPix, "&New", this),
            *open = new QAction(openPix, "&Open", this),
            *quit = new QAction(quitPix, "&Quit", this);
    quit->setShortcut(tr("CTRL+Q"));

    menuBar = QMainWindow::menuBar()->addMenu("&File");
    menuBar->addAction(newA);
    menuBar->addAction(open);
    menuBar->addSeparator();
    menuBar->addAction(quit);

    qApp->setAttribute(Qt::AA_DontShowIconsInMenus, false);

    connect(open, SIGNAL(triggered()), this, SLOT(open()));
    connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void MainMenu::open() {
    QFileDialog fileDialog;
    QString path = fileDialog.getOpenFileName();
    const std::string data = path.toStdString();
    std::cout << data << std::endl;
}
