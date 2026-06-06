#include <QApplication>
#include "menuwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MenuWindow menu;
    menu.show();

    return a.exec();
}