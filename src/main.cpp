#include "MainMenu.h"
#include "MyCanvas.h"
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

    something_Sane *meems = new something_Sane(window, QPoint(0, window->file->height()), QSize(100, 100));
    meems->show();

    return app.exec();
}
