#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gamescene.h"
#include "menuwindow.h"
#include <QGraphicsView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1. Fenstergröße exakt wie beim Hauptmenü einstellen (450 x 650)
    setWindowTitle("Block Blast");
    resize(450, 650);
    setMinimumSize(450, 650);
    setMaximumSize(450, 650); // Verhindert, dass das Fenster maximiert/verzerrt werden kann

    // 2. Spielszene erstellen
    GameScene *scene = new GameScene(this);

    // 3. View erstellen und anpassen
    QGraphicsView *view = new QGraphicsView(this);
    view->setScene(scene);

    // Ränder der View entfernen, damit das Spielfeld sauber im Fenster sitzt
    view->setFrameStyle(QFrame::NoFrame);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Die View als zentrales Element setzen (füllt jetzt exakt die 450x650 aus)
    setCentralWidget(view);

    // 4. Signal verbinden: Zurück zum Menü
    connect(scene, &GameScene::returnToMenuRequested, this, [this]() {
        MenuWindow *menu = new MenuWindow();
        menu->setAttribute(Qt::WA_DeleteOnClose);
        menu->show(); // Öffnet das perfekt proportionierte Hauptmenü

        this->close(); // Schließt dieses Spielfenster
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}