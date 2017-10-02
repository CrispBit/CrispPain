#include "MainMenu.h"
#include "CrispPainView.h"
#include <iostream>
#include <string>
#include <QApplication>
#include <QMenu>

int main(int argc, char** argv) {
    // select file and get path
    QApplication app(argc, argv);

    MainMenu *window = new MainMenu();
    window->resize(250, 150);
    window->setWindowTitle("Simple menu");
    window->show();

    CrispPainView *view = new something_Sane(window, QPoint(1, window->menuBar->height()), QSize(100, 100));
    view->show();

    return app.exec();
}
