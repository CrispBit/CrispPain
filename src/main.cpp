#include "MainMenu.h"
#include <iostream>
#include <string>
#include <QApplication>
#include <QMenu>

int main(int argc, char** argv) {
    // select file and get path
    QApplication app(argc, argv);

    MainMenu *window = new MainMenu();
    window->resize(800, 500);
    window->setWindowTitle("Simple menu");
    window->show();

    return app.exec();
}
