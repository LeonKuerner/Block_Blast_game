#include "gamescene.h"
#include <QApplication>
#include <QBrush>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsProxyWidget> // NEU: Benötigt für das Einbetten des Buttons
#include <QGraphicsRectItem>
#include <QMessageBox>
#include <QPen>
#include <QPushButton>
#include <QTimer>
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

    // --- NEU: DAUERHAFTER RESTART-BUTTON ---
    QPushButton *mainRestartButton = new QPushButton("Resart");
    // Ein bisschen Styling, damit er gut aussieht
    mainRestartButton->setFont(QFont("Arial", 12, QFont::Bold));
    mainRestartButton->setStyleSheet("QPushButton {"
                                     "   background-color: #FF0000;" // Schönes Grün
                                     "   color: white;"
                                     "   border-radius: 5px;"
                                     "   padding: 6px;"
                                     "}"
                                     "QPushButton:hover {"
                                     "   background-color: #8B0000;" // Dunkler beim Drüberfahren
                                     "}");
    mainRestartButton->setFixedWidth(120);

    // Button in die Scene einbetten
    QGraphicsProxyWidget *proxy = addWidget(mainRestartButton);

    // Positionieren: Unten zentriert (z.B. bei Y = 600, da die Szene 650 hoch ist)
    double buttonX = (400.0 - mainRestartButton->width()) / 2.0;
    proxy->setPos(buttonX, 600);
    proxy->setZValue(10); // Sicherstellen, dass der Button immer ganz oben liegt

    // Verbindung herstellen: Klick startet das Spiel neu
    connect(mainRestartButton, &QPushButton::clicked, this, &GameScene::restartGame);
    // ---------------------------------------

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
            // NEU: Sicherstellen, dass wir nicht aus Versehen das Proxy-Widget des Buttons löschen!
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
    std::vector<std::vector<std::vector<int>>> empty_template = {
        {{0}}
    };
    std::vector<std::vector<std::vector<int>>> templates = {
        {{1, 1}, {1, 1}},                   //2x2 quadrat
        {{0, 1}, {0, 1}, {1, 1}},           //gespiegeltes l stück
        {{1, 1}, {0, 1}, {0, 1}},           //l gespegelt umgedreht
        {{1, 1, 1}},                        //3x1 linie horizontal
        {{1, 1, 1}, {0, 1, 0}},             //t stück
        {{0, 1, 0}, {1, 1, 1}},             //umgedrehtes t stück
        {{1, 1}, {1, 1}, {1, 1}},           //2x3 block
        {{1}}                               //1x1 block
    };

    std::vector<std::vector<std::vector<int>>> templates_prio = {
        {{1, 0}, {1, 0}, {1, 1}},           //l stück
        {{1, 1}, {1, 0}, {1, 0}},           //l umgedreht
        {{1, 1, 1}, {1, 1, 1}},             //3x2 block
        {{1}, {1}, {1}},                    //1x3 linie vertikal

    };

    std::vector<std::string> colors = {"red", "blue", "green", "yellow", "orange"};

    int randomTamplate = rand() % 4;
    int randomColorIdx = rand() % colors.size();

    std::vector<std::vector<int>> shape;
    if( randomTamplate <= 1) {
        int randomShapeIdx = rand() % templates.size();
        shape = templates[randomShapeIdx];
    } else {
        int randomShapeIdx = rand() % templates_prio.size();
        shape = templates_prio[randomShapeIdx];
    }


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

        for (const auto &row : block.getShape()) {
            for (int val : row) {
                if (val != 0)
                    score += 1;
            }
        }

        int clearedLines = board.clearFullLines();
        if (clearedLines > 0) {
            score += clearedLines * 80;
        }

        updateScoreDisplay();

        int slot = draggedItem->getSlotIndex();
        removeItem(draggedItem);
        delete draggedItem;
        UIBlocks[slot] = nullptr;
        slotOccupied[slot] = false;

        drawBoard();

        if (!slotOccupied[0] && !slotOccupied[1] && !slotOccupied[2]) {
            spawnNewBlocks();
        }

        checkGameOver();
    } else {
        draggedItem->resetPosition();
    }
}

void GameScene::restartGame()
{
    QList<QGraphicsItem *> allItems = items();
    for (QGraphicsItem *item : allItems) {
        QGraphicsTextItem *textItem = dynamic_cast<QGraphicsTextItem *>(item);
        if (textItem && textItem->toPlainText() == "GAME OVER") {
            removeItem(textItem);
            delete textItem;
        }
    }

    score = 0;
    updateScoreDisplay();
    board.reset();

    for (int i = 0; i < 3; ++i) {
        if (UIBlocks[i]) {
            removeItem(UIBlocks[i]);
            delete UIBlocks[i];
            UIBlocks[i] = nullptr;
        }
        slotOccupied[i] = false;
    }

    drawBoard();
    spawnNewBlocks();
}

void GameScene::checkGameOver()
{
    bool movePossible = false;

    for (int i = 0; i < 3; ++i) {
        if (slotOccupied[i] && UIBlocks[i]) {
            Block block = UIBlocks[i]->getBlockData();

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
        gameOverText->setDefaultTextColor(QColor(230, 50, 50));
        gameOverText->setFont(QFont("Impact", 40, QFont::Bold));

        double textWidth = gameOverText->boundingRect().width();
        double textHeight = gameOverText->boundingRect().height();

        double centerX = (400.0 - textWidth) / 2.0;
        double centerY = (650.0 - textHeight) / 2.0;

        gameOverText->setPos(centerX, centerY);

        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
        shadow->setBlurRadius(8);
        shadow->setColor(QColor(0, 0, 0, 180));
        shadow->setOffset(4, 4);

        gameOverText->setGraphicsEffect(shadow);
        addItem(gameOverText);
    }
}
