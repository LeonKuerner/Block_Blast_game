#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include <QWidget>

class QPushButton;

class MenuWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MenuWindow(QWidget *parent = nullptr);

private slots:
    void startGame();

private:
    QPushButton *startButton;
    QPushButton *exitButton;
};

#endif