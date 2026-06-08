#include "menuwindow.h"
#include "mainwindow.h"
#include "gamescene.h"

#include <QGraphicsView>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFont>
#include <QApplication>

MenuWindow::MenuWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Block Blast - Main Menu");
    resize(450, 650);

    QLabel *title = new QLabel("Block Blast");
    title->setAlignment(Qt::AlignCenter);
    title->setFont(QFont("Arial", 32, QFont::Bold));

    startButton = new QPushButton("Spiel starten");
    startButton->setFixedHeight(50);

    exitButton = new QPushButton("Beenden");
    exitButton->setFixedHeight(50);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addStretch();
    layout->addWidget(title);
    layout->addSpacing(40);
    layout->addWidget(startButton);
    layout->addWidget(exitButton);
    layout->addStretch();

    setStyleSheet(
        "QWidget { background-color: #f4f4f4; }"
        "QPushButton {"
        "  font-size: 18px;"
        "  font-weight: bold;"
        "  border-radius: 8px;"
        "  background-color: #4285F4;"
        "  color: white;"
        "}"
        "QPushButton:hover {"
        "  background-color: #2b65c8;"
        "}"
        "QLabel { color: #222; }");

    connect(startButton, &QPushButton::clicked, this, &MenuWindow::startGame);
    connect(exitButton, &QPushButton::clicked, qApp, &QApplication::quit);
}

void MenuWindow::startGame()
{
    // HIER WAR DER FEHLER: Wir erstellen jetzt das richtige MainWindow
    MainWindow *mainWindow = new MainWindow();
    mainWindow->setAttribute(Qt::WA_DeleteOnClose); // Gibt den Speicher beim Schließen frei
    mainWindow->show(); // Zeigt das eigentliche Spielfenster an

    close(); // Schließt das Hauptmenü
}