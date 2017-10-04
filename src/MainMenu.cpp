#include "MainMenu.h"
#include "CrispPainView.h"
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QToolBar>
#include <iostream>

MainMenu::MainMenu(QWidget *parent) : QMainWindow(parent) {
    // set up menu bar
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

    // set up main view
    cpview = new CrispPainView(this, QPoint(1, menuBar->height()),
            QSize(10, 10));
            //QSize(800 - 2 - 80, 500 - 1 - menuBar->height()));
    setCentralWidget(cpview);

    QToolBar *toolbar = new QToolBar("toolbar");
    QAction *meems = toolbar->addAction("meems");
    this->addToolBar(Qt::RightToolBarArea, toolbar);

    connect(open, SIGNAL(triggered()), this, SLOT(open()));
    connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(meems, SIGNAL(triggered()), this, SLOT(meems()));
}

void MainMenu::meems() {
    std::cout << "meems" << std::endl;
}

void MainMenu::open() {
    QFileDialog fileDialog;
    QString path = fileDialog.getOpenFileName();
    const std::string data = path.toStdString();
    std::cout << data << std::endl;
}
