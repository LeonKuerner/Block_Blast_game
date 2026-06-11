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
#include <QSettings>      // HIER ERGÄNZT
#include <QInputDialog>   // HIER ERGÄNZT
#include <QCoreApplication>// HIER ERGÄNZT
#include <QDir>           // HIER ERGÄNZT

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>

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

    QPushButton *menuButton = new QPushButton("↩");
    menuButton->setFont(QFont("Arial", 14, QFont::Bold));
    menuButton->setFixedSize(30, 30);
    menuButton->setStyleSheet("QPushButton {"
                              "   background-color: #555555;"
                              "   color: white;"
                              "   border-radius: 5px;"
                              "   padding: 0px;"
                              "}"
                              "QPushButton:hover {"
                              "   background-color: #333333;"
                              "}");

    QGraphicsProxyWidget *menuProxy = addWidget(menuButton);
    menuProxy->setPos(340, 15);
    menuProxy->setZValue(10);

    connect(menuButton, &QPushButton::clicked, this, [this]() {
        emit returnToMenuRequested();
    });

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

QList<GameScene::HighscoreEntry> GameScene::loadHighscores()
{
    // Bestimmt den Pfad direkt im Git / Build-Ordner der ausführbaren Datei
    QString iniPath = QDir(QCoreApplication::applicationDirPath()).filePath("highscores.ini");
    QSettings settings(iniPath, QSettings::IniFormat);
    QList<HighscoreEntry> highscores;

    for (int i = 0; i < 5; ++i) {
        QString name = settings.value(QString("Highscore/Rank_%1_Name").arg(i), "Unbekannt").toString();
        int scoreVal = settings.value(QString("Highscore/Rank_%1_Score").arg(i), 0).toInt();
        highscores.append({name, scoreVal});
    }
    return highscores;
}

void GameScene::checkAndSaveHighscore()
{
    QList<HighscoreEntry> highscores = loadHighscores();

    // Falls die Liste leer ist, füllen wir sie temporär auf, damit .last() nicht abstürzt
    while(highscores.size() < 5) {
        highscores.append({"Unbekannt", 0});
    }

    if (score > highscores.last().score || highscores.size() < 5) {
        bool ok;
        QString spielerName = QInputDialog::getText(nullptr,
                                                    "Neuer Highscore!",
                                                    "Herzlichen Glückwunsch! Trage deinen Namen ein:",
                                                    QLineEdit::Normal,
                                                    "Spieler", &ok);

        if (!ok || spielerName.isEmpty()) {
            spielerName = "Anonym";
        }

        highscores.append({spielerName, score});

        std::sort(highscores.begin(), highscores.end(), [](const HighscoreEntry &a, const HighscoreEntry &b) {
            return a.score > b.score;
        });

        while (highscores.size() > 5) {
            highscores.removeLast();
        }

        QString iniPath = QDir(QCoreApplication::applicationDirPath()).filePath("highscores.ini");
        QSettings settings(iniPath, QSettings::IniFormat);
        for (int i = 0; i < highscores.size(); ++i) {
            settings.setValue(QString("Highscore/Rank_%1_Name").arg(i), highscores[i].name);
            settings.setValue(QString("Highscore/Rank_%1_Score").arg(i), highscores[i].score);
        }
    }
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
                                   delete effect;
                               });
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
        else
        {
            checkGameOver();
        }
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
        // HIER GEÄNDERT: Erst die Slots auf false setzen, um die Endlosschleife beim restartGame() zu brechen!
        for(int i = 0; i < 3; ++i) {
            slotOccupied[i] = false;
        }

        // 1. Highscore prüfen, ggf. nach Name fragen und speichern
        checkAndSaveHighscore();

        QMessageBox gameOverBox;
        gameOverBox.setStyleSheet(
            "QMessageBox {"
            "   background-color: #2c3e50;"
            "   color: white;"
            "   font-family: 'Arial';"
            "}"
            "QLabel {"
            "   color: white;"
            "   font-size: 16px;"
            "   qproperty-alignment: 'AlignCenter';"
            "   min-width: 280px;"
            "}"
            "QMessageBox QDialogButtonBox {"
            "   qproperty-centerButtons: true;"
            "}"
            "QPushButton {"
            "   background-color: #e63232;"
            "   color: white;"
            "   font-size: 14px;"
            "   font-weight: bold;"
            "   padding: 8px 24px;"
            "   border-radius: 4px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #ff4d4d;"
            "}"
            );

        // 2. Aktuelle Top 5 mit Namen laden
        QList<HighscoreEntry> scores = loadHighscores();
        QString highscoreListText = "<br><b>🏆 TOP 5 BESTENLISTE 🏆</b><br><table align='center' border='0' cellspacing='5'>";

        for(int i = 0; i < scores.size(); ++i) {
            if(scores[i].score > 0) {
                highscoreListText += QString("<tr><td><b>%1.</b></td><td>%2</td><td><b>%3</b> Pkt.</td></tr>")
                .arg(i+1).arg(scores[i].name).arg(scores[i].score);
            }
        }
        highscoreListText += "</table>";

        QString message = QString("<center><font size='6' color='#e63232'><b>GAME OVER</b></font><br><br>"
                                  "Deine erreichte Punktzahl:<br>"
                                  "<font size='5'><b>%1</b></font><br>"
                                  "%2</center>").arg(score).arg(highscoreListText);

        gameOverBox.setInformativeText(message);
        gameOverBox.addButton(QMessageBox::Ok);
        gameOverBox.exec();

        restartGame();
    }
}