#include "gamescene.h"
#include <QBrush>
#include <QGraphicsRectItem>
#include <QPen>
#include <cstdlib>
#include <ctime>

GameScene::GameScene(QObject *parent)
    : QGraphicsScene(parent)
    , score(0)
{
    srand(static_cast<unsigned int>(time(nullptr))); //random seed
    setSceneRect(0, 0, 400, 650);

    for (int i = 0; i < 3; ++i) {
        UIBlocks[i] = nullptr;
        slotOccupied[i] = false;
    }

    //textfeld für score erstellen
    scoreDisplay = new QGraphicsTextItem();
    scoreDisplay->setDefaultTextColor(Qt::black);
    scoreDisplay->setFont(QFont("Arial", 16, QFont::Bold));
    scoreDisplay->setPos(40, 15);
    addItem(scoreDisplay);
    updateScoreDisplay();

    drawBoard();
    spawnNewBlocks();
}

void GameScene::updateScoreDisplay()
{
    scoreDisplay->setPlainText("Punkte: " + QString::number(score));
}

void GameScene::drawBoard()
{
    //sucht nach QGraphicsRectItem die keine child items sind
    QList<QGraphicsItem *> allItems = items();
    for (QGraphicsItem *item : allItems) {
        if (item != scoreDisplay && !dynamic_cast<BlockItem *>(item)) {
            if (dynamic_cast<QGraphicsRectItem *>(item) && item->parentItem() == nullptr) {
                removeItem(item);
                delete item;
            }
        }
    }

    std::vector<std::vector<int>> grid = board.getGrid();

    //spielfeld zeichnen
    for (size_t r = 0; r < grid.size(); ++r) {
        for (size_t c = 0; c < grid[r].size(); ++c) {
            int x = offsetX + (c * cellSize);
            int y = offsetY + (r * cellSize);

            QGraphicsRectItem *rect = new QGraphicsRectItem(x, y, cellSize, cellSize);
            QPen pen(QColor(220, 220, 220));
            rect->setPen(pen);

            if (grid[r][c] == 0) {
                rect->setBrush(QBrush(QColor(245, 245, 245))); // Leeres Feld
            } else {
                QColor blockColor;
                switch (grid[r][c]) {
                case 1:
                    blockColor = QColor(230, 50, 50);
                    break; //red
                case 2:
                    blockColor = QColor(0, 120, 215);
                    break; //blue
                case 3:
                    blockColor = QColor(50, 180, 50);
                    break; //green
                case 4:
                    blockColor = QColor(240, 200, 0);
                    break; //yellow
                case 5:
                    blockColor = QColor(255, 130, 0);
                    break; //orange
                default:
                    blockColor = QColor(128, 128, 128);
                    break;
                }
                rect->setBrush(QBrush(blockColor));
            }
            //z value -1 damit es im hintergrund ist
            rect->setZValue(-1);
            addItem(rect);
        }
    }
}

Block GameScene::generateRandomBlockData()
{
    std::vector<std::vector<std::vector<int>>> templates = {
        {{1, 1}, {1, 1}}, //2x2 quadrat

        {{1, 1, 1}}, //3x1 linie horizontal

        {{1}, {1}, {1}}, //1x3 linie vertikal

        {{1, 0}, {1, 0}, {1, 1}}, //l stück

        {{1, 1, 1}, {0, 1, 0}}, //t stück

        {{1}} //1x1 block
    };
    std::vector<std::string> colors = {"red", "blue", "green", "yellow", "orange"};

    int randomShapeIdx = rand() % templates.size();
    int randomColorIdx = rand() % colors.size();

    auto shape = templates[randomShapeIdx];
    int colorId = randomColorIdx + 1;
    for (size_t r = 0; r < shape.size(); ++r) {
        for (size_t c = 0; c < shape[r].size(); ++c) {
            if (shape[r][c] != 0)
                shape[r][c] = colorId;
        }
    }

    return Block(colors[randomColorIdx], shape);
}
void GameScene::spawnNewBlocks()
{
    if (slotOccupied[0] || slotOccupied[1] || slotOccupied[2])
        return;

    int slotXPositions[3] = {40, 160, 280};
    int slotYPosition = 450;

    for (int i = 0; i < 3; ++i) {
        Block data = generateRandomBlockData();
        BlockItem *item = new BlockItem(data, i);

        item->setPos(slotXPositions[i], slotYPosition);
        item->saveCurrentPosition();

        addItem(item);
        UIBlocks[i] = item;
        slotOccupied[i] = true;
    }

    checkGameOver();
}

void GameScene::handleBlockPlacement(BlockItem *draggedItem)
{
    if (!draggedItem)
        return;

    //get block position
    QPointF itemPos = draggedItem->pos();
    Block block = draggedItem->getBlockData();
    auto shape = block.getShape();

    int firstRowWithBlock = -1;
    int firstColWithBlock = -1;

    for (size_t r = 0; r < shape.size(); ++r) {
        for (size_t c = 0; c < shape[r].size(); ++c) {
            if (shape[r][c] != 0) {
                firstRowWithBlock = r;
                firstColWithBlock = c;
                break;
            }
        }
        if (firstRowWithBlock != -1)
            break;
    }

    double realX = itemPos.x() + (firstColWithBlock * 25) + 12.5;
    double realY = itemPos.y() + (firstRowWithBlock * 25) + 12.5;

    int targetCol = static_cast<int>(std::floor((realX - offsetX) / cellSize)) - firstColWithBlock;
    int targetRow = static_cast<int>(std::floor((realY - offsetY) / cellSize)) - firstRowWithBlock;

    if (board.canPlaceBlock(block, targetRow, targetCol)) {
        board.placeBlock(block, targetRow, targetCol);

        //punkte für gesetzten block berechnen
        for (const auto &row : block.getShape()) {
            for (int val : row) {
                if (val != 0)
                    score += 10;
            }
        }

        //clear lines gibt punkte
        int clearedLines = board.clearFullLines();
        if (clearedLines > 0) {
            score += clearedLines * 100;
        }

        updateScoreDisplay();

        //gesetzten block aufräumen
        int slot = draggedItem->getSlotIndex();
        removeItem(draggedItem);
        delete draggedItem;
        UIBlocks[slot] = nullptr;
        slotOccupied[slot] = false;

        //spielfeld aktualisieren
        drawBoard();

        //wenn auswahl leer 3 neue blöcke
        if (!slotOccupied[0] && !slotOccupied[1] && !slotOccupied[2]) {
            spawnNewBlocks();
        }

        checkGameOver();
    } else {
        draggedItem->resetPosition();
    }
}
void GameScene::checkGameOver()
{
    bool movePossible = false;

    for (int i = 0; i < 3; ++i) {
        if (slotOccupied[i] && UIBlocks[i]) {
            Block block = UIBlocks[i]->getBlockData();

            //prüfen ob es noch platz gibt
            for (int r = 0; r < 8; ++r) {
                for (int c = 0; c < 8; ++c) {
                    if (board.canPlaceBlock(block, r, c)) {
                        movePossible = true;
                        break;
                    }
                }
                if (movePossible)
                    break;
            }
        }
        if (movePossible)
            break;
    }

    if (!movePossible && (slotOccupied[0] || slotOccupied[1] || slotOccupied[2])) {
        QGraphicsTextItem *gameOverText = new QGraphicsTextItem("GAME OVER");
        gameOverText->setDefaultTextColor(Qt::red);
        gameOverText->setFont(QFont("Arial", 32, QFont::Bold));
        gameOverText->setPos(70, 240);
        addItem(gameOverText);
    }
}