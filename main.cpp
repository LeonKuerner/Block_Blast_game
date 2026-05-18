#include <QApplication>
#include <QGraphicsView>
#include "gamescene.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 1. Erstelle die Szene (unsere Logik + Grafik-Inhalt)
    GameScene *scene = new GameScene();

    // 2. Erstelle die View (das eigentliche UI-Fenster, das die Szene anzeigt)
    QGraphicsView view(scene);
    view.setWindowTitle("Block Blast Klon in Qt");
    view.resize(450, 650); // Etwas größer als die Szene selbst
    view.show();

    return a.exec();
}