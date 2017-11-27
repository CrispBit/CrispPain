#include "MainMenu.h"
#include <crispsynth/Locator.h>
#include <iostream>
#include <string>
#include <QApplication>
#include <QMenu>

int main(int argc, char** argv) {
    Locator::provideArgs(argv[0]);
    Locator::provideResourcesService(std::make_unique<LocalResources>());

    // select file and get path
    QApplication app(argc, argv);

    MainMenu window;
    window.resize(800, 500);
    window.setWindowTitle("Simple menu");
    window.show();

    return app.exec();
}
