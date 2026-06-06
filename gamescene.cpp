#include "gamescene.h"

#include <QApplication>
#include <QBrush>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsEllipseItem>
#include <QGraphicsWidget>
#include <QGraphicsProxyWidget>
#include <QGraphicsRectItem>
#include <QMessageBox>
#include <QPen>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QRandomGenerator>
#include <QTimer>

#include <cmath>
#include <cstdlib>
#include <ctime>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void createFirework(GameScene *scene, QPointF center)
{
    for (int i = 0; i < 20; i++)
    {
        QColor color = QColor::fromHsv(
            QRandomGenerator::global()->bounded(360),
            255,
            255);

        QGraphicsWidget *particle = new QGraphicsWidget();

        QGraphicsEllipseItem *ellipse =
            new QGraphicsEllipseItem(0, 0, 8, 8, particle);

        ellipse->setPen(QPen(Qt::NoPen));
        ellipse->setBrush(QBrush(color));

        particle->setPos(center);
        particle->setZValue(40);
        scene->addItem(particle);

        double angle = (2.0 * M_PI * i) / 20.0;
        double distance = QRandomGenerator::global()->bounded(50, 120);

        QPointF target(
            center.x() + std::cos(angle) * distance,
            center.y() + std::sin(angle) * distance);

        QPropertyAnimation *anim = new QPropertyAnimation(particle, "pos");
        anim->setDuration(800);
        anim->setStartValue(center);
        anim->setEndValue(target);
        anim->setEasingCurve(QEasingCurve::OutCubic);

        QObject::connect(anim, &QPropertyAnimation::finished,
                         [scene, particle]()
                         {
                             scene->removeItem(particle);
                             delete particle;
                         });

        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

GameScene::GameScene(QObject *parent)
    : QGraphicsScene(parent), score(0)
{
    srand(static_cast<unsigned int>(time(nullptr)));
    setSceneRect(0, 0, 400, 650);

    for (int i = 0; i < 3; ++i)
    {
        UIBlocks[i] = nullptr;
        slotOccupied[i] = false;
    }

    scoreDisplay = new QGraphicsTextItem();
    scoreDisplay->setDefaultTextColor(Qt::black);
    scoreDisplay->setFont(QFont("Arial", 16, QFont::Bold));
    scoreDisplay->setPos(40, 15);
    addItem(scoreDisplay);
    updateScoreDisplay();

    QPushButton *mainRestartButton = new QPushButton("Restart");
    mainRestartButton->setFont(QFont("Arial", 12, QFont::Bold));
    mainRestartButton->setStyleSheet("QPushButton {"
                                     "   background-color: #FF0000;"
                                     "   color: white;"
                                     "   border-radius: 5px;"
                                     "   padding: 6px;"
                                     "}"
                                     "QPushButton:hover {"
                                     "   background-color: #8B0000;"
                                     "}");
    mainRestartButton->setFixedWidth(120);

    QGraphicsProxyWidget *proxy = addWidget(mainRestartButton);

    double buttonX = (400.0 - mainRestartButton->width()) / 2.0;
    proxy->setPos(buttonX, 600);
    proxy->setZValue(10);

    connect(mainRestartButton, &QPushButton::clicked, this, &GameScene::restartGame);

    drawBoard();
    spawnNewBlocks();
}

void GameScene::updateScoreDisplay()
{
    scoreDisplay->setPlainText("Punkte: " + QString::number(score));
}

void GameScene::drawBoard()
{
    QList<QGraphicsItem *> allItems = items();

    for (QGraphicsItem *item : allItems)
    {
        if (item != scoreDisplay && !dynamic_cast<BlockItem *>(item))
        {
            if (dynamic_cast<QGraphicsRectItem *>(item) && item->parentItem() == nullptr)
            {
                removeItem(item);
                delete item;
            }
        }
    }

    std::vector<std::vector<int>> grid = board.getGrid();

    for (size_t r = 0; r < grid.size(); ++r)
    {
        for (size_t c = 0; c < grid[r].size(); ++c)
        {
            int x = offsetX + (c * cellSize);
            int y = offsetY + (r * cellSize);

            QGraphicsRectItem *rect = new QGraphicsRectItem(x, y, cellSize, cellSize);
            QPen pen(QColor(220, 220, 220));
            rect->setPen(pen);

            if (grid[r][c] == 0)
            {
                rect->setBrush(QBrush(QColor(245, 245, 245)));
            }
            else
            {
                QColor blockColor;

                switch (grid[r][c])
                {
                case 1:
                    blockColor = QColor(230, 50, 50);
                    break;
                case 2:
                    blockColor = QColor(0, 120, 215);
                    break;
                case 3:
                    blockColor = QColor(50, 180, 50);
                    break;
                case 4:
                    blockColor = QColor(240, 200, 0);
                    break;
                case 5:
                    blockColor = QColor(255, 130, 0);
                    break;
                default:
                    blockColor = QColor(128, 128, 128);
                    break;
                }

                rect->setBrush(QBrush(blockColor));
            }

            rect->setZValue(-1);
            addItem(rect);
        }
    }
}

Block GameScene::generateRandomBlockData()
{
    std::vector<std::vector<std::vector<int>>> templates = {
        {{1, 1}, {1, 1}},
        {{0, 1}, {0, 1}, {1, 1}},
        {{1, 1}, {0, 1}, {0, 1}},
        {{1, 1, 1}},
        {{1, 1, 1}, {0, 1, 0}},
        {{0, 1, 0}, {1, 1, 1}},
        {{1, 1}, {1, 1}, {1, 1}},
        {{1}}};

    std::vector<std::vector<std::vector<int>>> templates_prio = {
        {{1, 0}, {1, 0}, {1, 1}},
        {{1, 1}, {1, 0}, {1, 0}},
        {{1, 1, 1}, {1, 1, 1}},
        {{1}, {1}, {1}}};

    std::vector<std::string> colors = {"red", "blue", "green", "yellow", "orange"};

    int randomTemplate = rand() % 4;
    int randomColorIdx = rand() % colors.size();

    std::vector<std::vector<int>> shape;

    if (randomTemplate <= 1)
    {
        int randomShapeIdx = rand() % templates.size();
        shape = templates[randomShapeIdx];
    }
    else
    {
        int randomShapeIdx = rand() % templates_prio.size();
        shape = templates_prio[randomShapeIdx];
    }

    int colorId = randomColorIdx + 1;

    for (size_t r = 0; r < shape.size(); ++r)
    {
        for (size_t c = 0; c < shape[r].size(); ++c)
        {
            if (shape[r][c] != 0)
            {
                shape[r][c] = colorId;
            }
        }
    }

    return Block(colors[randomColorIdx], shape);
}

void GameScene::spawnNewBlocks()
{
    if (slotOccupied[0] || slotOccupied[1] || slotOccupied[2])
    {
        return;
    }

    int slotXPositions[3] = {40, 160, 280};
    int slotYPosition = 450;

    for (int i = 0; i < 3; ++i)
    {
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
    {
        return;
    }

    QPointF itemPos = draggedItem->pos();
    Block block = draggedItem->getBlockData();
    auto shape = block.getShape();

    int firstRowWithBlock = -1;
    int firstColWithBlock = -1;

    for (size_t r = 0; r < shape.size(); ++r)
    {
        for (size_t c = 0; c < shape[r].size(); ++c)
        {
            if (shape[r][c] != 0)
            {
                firstRowWithBlock = r;
                firstColWithBlock = c;
                break;
            }
        }

        if (firstRowWithBlock != -1)
        {
            break;
        }
    }

    double realX = itemPos.x() + (firstColWithBlock * 25) + 12.5;
    double realY = itemPos.y() + (firstRowWithBlock * 25) + 12.5;

    int targetCol = static_cast<int>(std::floor((realX - offsetX) / cellSize)) - firstColWithBlock;
    int targetRow = static_cast<int>(std::floor((realY - offsetY) / cellSize)) - firstRowWithBlock;

    if (board.canPlaceBlock(block, targetRow, targetCol))
    {
        board.placeBlock(block, targetRow, targetCol);

        for (const auto &row : block.getShape())
        {
            for (int val : row)
            {
                if (val != 0)
                {
                    score += 1;
                }
            }
        }

        int clearedLines = board.clearFullLines();

        if (clearedLines > 0)
        {
            score += clearedLines * 80;

            QString effectText;

            if (clearedLines == 1)
            {
                effectText = "Nice!";
            }
            else if (clearedLines == 2)
            {
                effectText = "Great!";
            }
            else
            {
                effectText = "Amazing!";
            }

            QGraphicsTextItem *effect = addText(effectText);
            effect->setDefaultTextColor(Qt::yellow);
            effect->setFont(QFont("Arial", 30, QFont::Bold));
            effect->setPos(120, 235);
            effect->setZValue(50);

            createFirework(this, QPointF(200, 270));

            QTimer::singleShot(1000, [this, effect]()
                               {
                                   removeItem(effect);
                                   delete effect; });
        }

        updateScoreDisplay();

        int slot = draggedItem->getSlotIndex();
        removeItem(draggedItem);
        delete draggedItem;
        UIBlocks[slot] = nullptr;
        slotOccupied[slot] = false;

        drawBoard();

        if (!slotOccupied[0] && !slotOccupied[1] && !slotOccupied[2])
        {
            spawnNewBlocks();
        }

        checkGameOver();
    }
    else
    {
        draggedItem->resetPosition();
    }
}

void GameScene::restartGame()
{
    QList<QGraphicsItem *> allItems = items();

    for (QGraphicsItem *item : allItems)
    {
        QGraphicsTextItem *textItem = dynamic_cast<QGraphicsTextItem *>(item);

        if (textItem && textItem->toPlainText() == "GAME OVER")
        {
            removeItem(textItem);
            delete textItem;
        }
    }

    score = 0;
    updateScoreDisplay();
    board.reset();

    for (int i = 0; i < 3; ++i)
    {
        if (UIBlocks[i])
        {
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

    for (int i = 0; i < 3; ++i)
    {
        if (slotOccupied[i] && UIBlocks[i])
        {
            Block block = UIBlocks[i]->getBlockData();

            for (int r = 0; r < 8; ++r)
            {
                for (int c = 0; c < 8; ++c)
                {
                    if (board.canPlaceBlock(block, r, c))
                    {
                        movePossible = true;
                        break;
                    }
                }

                if (movePossible)
                {
                    break;
                }
            }
        }

        if (movePossible)
        {
            break;
        }
    }

    if (!movePossible && (slotOccupied[0] || slotOccupied[1] || slotOccupied[2]))
    {
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