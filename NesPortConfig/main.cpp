#include "NesPortConfig.h"
#include <QtWidgets/QApplication>
#include <QAbstractNativeEventFilter>
#include <SDL3/SDL.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    NesPortConfig window;
    window.show();
    return app.exec();
}
