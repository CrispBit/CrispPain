#include "MainMenu.h"
#include "CrispPainView.h"
#include <crispsynth/Locator.h>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QToolBar>
#include <QLineEdit>
#include <QHeaderView>
#include <QStringList>
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

    QToolBar *toolbar = new QToolBar("toolbar");
    QToolBar *propertiesPane = new QToolBar("properties");
    QAction *addHurtbox = toolbar->addAction(QIcon((Locator::rootPath / "assets/images/add-hurtbox.png").c_str()), "add hurtbox");
    hurtboxTable = new QTableWidget(propertiesPane);

    this->addToolBar(Qt::RightToolBarArea, toolbar);
    this->addToolBar(Qt::BottomToolBarArea, propertiesPane);
    propertiesPane->addWidget(hurtboxTable);

    // set up main view
    cpview = new CrispPainView(this, QPoint(1, menuBar->height()),
            QSize(10, 10));
    setCentralWidget(cpview->qwidget);

    hurtboxTable->setColumnCount(10);
    hurtboxTable->verticalHeader()->setVisible(false);
    hurtboxTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    QStringList headerLabels;
    headerLabels << "id" << "name" << "r" << "h" <<  "x" << "y"
        << "z" << "sX" << "sY" << "sZ";
    hurtboxTable->setHorizontalHeaderLabels(headerLabels);
    hurtboxTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    cpview->boxCollection = Locator::getResource()->loadBoxes("meshes/mfw", "crispbit.crispbox");
    cpview->createHurtboxTable(hurtboxTable);

    connect(open, SIGNAL(triggered()), this, SLOT(open()));
    connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(addHurtbox, &QAction::triggered, this, [this]{createHurtbox(hurtboxTable);});
}

void MainMenu::createHurtbox(QTableWidget *table) {
    cpview->createHurtbox(table);
}

void MainMenu::open() {
    QFileDialog fileDialog;
    QString path = fileDialog.getOpenFileName();
    const std::string data = path.toStdString();
    std::cout << data << std::endl;
}
