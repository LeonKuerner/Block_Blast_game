#include <QApplication>
#include <QGraphicsView>
#include "gamescene.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //szene erstellen
    GameScene *scene = new GameScene();

    //view ist eigentliches ui window
    QGraphicsView view(scene);
    view.setWindowTitle("Block Blast");
    view.resize(450, 650);
    view.show();

    return a.exec();
}
