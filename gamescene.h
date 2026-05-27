#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include "block.h"
#include "blockitem.h"
#include "gameboard.h"

class GameScene : public QGraphicsScene
{
    Q_OBJECT

private:
    GameBoard board;
    BlockItem *UIBlocks[3];
    bool slotOccupied[3];

    int score;
    QGraphicsTextItem *scoreDisplay;

    const int cellSize = 40;
    const int offsetX = 40;
    const int offsetY = 60;

    void drawBoard();
    void updateScoreDisplay();
    void spawnNewBlocks();
    Block generateRandomBlockData();
    void checkGameOver();

public:
    GameScene(QObject *parent = nullptr);
    void restartGame();
    //wird aufgerufen wenn block losgelassen wird
    void handleBlockPlacement(BlockItem *draggedItem);
};

#endif // GAME_SCENE_H