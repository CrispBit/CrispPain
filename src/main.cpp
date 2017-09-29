#include "MainMenu.h"
#include <iostream>
#include <string>
#include <QApplication>

int main(int argc, char** argv) {
    // select file and get path
    QApplication app(argc, argv);

    MainMenu window;
    window.resize(250, 150);
    window.setWindowTitle("Simple menu");
    window.show();

    return app.exec();
}
