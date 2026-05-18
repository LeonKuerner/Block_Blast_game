#include "gamescene.h"
#include <QGraphicsRectItem>
#include <QBrush>
#include <QPen>
#include <cstdlib>
#include <ctime>

GameScene::GameScene(QObject *parent)
    : QGraphicsScene(parent), score(0)
{
    srand(static_cast<unsigned int>(time(nullptr))); // Random Seed
    setSceneRect(0, 0, 400, 650);

    for (int i = 0; i < 3; ++i) {
        UIBlocks[i] = nullptr;
        slotOccupied[i] = false;
    }

    // Textfeld für Score erstellen
    scoreDisplay = new QGraphicsTextItem();
    scoreDisplay->setDefaultTextColor(Qt::black);
    scoreDisplay->setFont(QFont("Arial", 16, QFont::Bold));
    scoreDisplay->setPos(40, 15);
    addItem(scoreDisplay);
    updateScoreDisplay();

    drawBoard();
    spawnNewBlocks();
}

void GameScene::updateScoreDisplay() {
    scoreDisplay->setPlainText("Punkte: " + QString::number(score));
}

void GameScene::drawBoard()
{
    // FIX: Nur die Quadrate des Spielfelds löschen, nicht blind alles!
    // Wir können gezielt nach QGraphicsRectItem suchen, die KEINE Child-Items sind
    QList<QGraphicsItem*> allItems = items();
    for (QGraphicsItem* item : allItems) {
        if (item != scoreDisplay && !dynamic_cast<BlockItem*>(item)) {
            // Sicherstellen, dass es ein reines Hintergrundquadrat ist
            if (dynamic_cast<QGraphicsRectItem*>(item) && item->parentItem() == nullptr) {
                removeItem(item);
                delete item;
            }
        }
    }

    std::vector<std::vector<int>> grid = board.getGrid();

    // 8x8 Spielfeld zeichnen
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
                case 1: blockColor = QColor(230, 50, 50); break;   // red
                case 2: blockColor = QColor(0, 120, 215); break;   // blue
                case 3: blockColor = QColor(50, 180, 50); break;   // green
                case 4: blockColor = QColor(240, 200, 0); break;   // yellow
                case 5: blockColor = QColor(255, 130, 0); break;   // orange
                default: blockColor = QColor(128, 128, 128); break;
                }
                rect->setBrush(QBrush(blockColor));
            }
            // Z-Value auf -1 setzen, damit das Spielfeld IMMER hinter den ziehbaren Blöcken liegt
            rect->setZValue(-1);
            addItem(rect);
        }
    }
}

Block GameScene::generateRandomBlockData() {
    // FIX: Templates so kompakt wie möglich definieren (keine unnötigen leeren Zeilen/Spalten außen)
    std::vector<std::vector<std::vector<int>>> templates = {
        {{1, 1},
         {1, 1}},                 // 2x2 Quadrat

        {{1, 1, 1}},              // 3x1 Linie horizontal

        {{1},
         {1},
         {1}},                    // 1x3 Linie vertikal

        {{1, 0},
         {1, 0},
         {1, 1}},                 // L-Stück (Bündig links)

        {{1, 1, 1},
         {0, 1, 0}},              // T-Stück (Bündig oben)

        {{1}}                     // Einzelner Punkt
    };
    std::vector<std::string> colors = {"red", "blue", "green", "yellow", "orange"};

    int randomShapeIdx = rand() % templates.size();
    int randomColorIdx = rand() % colors.size();

    auto shape = templates[randomShapeIdx];
    int colorId = randomColorIdx + 1;
    for (size_t r = 0; r < shape.size(); ++r) {
        for (size_t c = 0; c < shape[r].size(); ++c) {
            if (shape[r][c] != 0) shape[r][c] = colorId;
        }
    }

    return Block(colors[randomColorIdx], shape);
}
void GameScene::spawnNewBlocks() {
    if (slotOccupied[0] || slotOccupied[1] || slotOccupied[2]) return;

    int slotXPositions[3] = {40, 160, 280};
    int slotYPosition = 450;

    for (int i = 0; i < 3; ++i) {
        Block data = generateRandomBlockData();
        BlockItem* item = new BlockItem(data, i);

        item->setPos(slotXPositions[i], slotYPosition);
        item->saveCurrentPosition();

        addItem(item);
        UIBlocks[i] = item;
        slotOccupied[i] = true;
    }

    checkGameOver();
}

// Wird direkt und zuverlässig aus dem mouseReleaseEvent von BlockItem getriggert!
void GameScene::handleBlockPlacement(BlockItem* draggedItem) {
    if (!draggedItem) return;

    // Wir holen uns die aktuelle Position
    QPointF itemPos = draggedItem->pos();
    Block block = draggedItem->getBlockData();
    auto shape = block.getShape();

    // Finden wir heraus, wo der ERSTE echte Blockteil (1) in der Matrix liegt.
    // Bei kompakten Templates ist das fast immer (0,0), aber sicher ist sicher:
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
        if (firstRowWithBlock != -1) break;
    }

    // Da das Item in der Auslage mit cellSize=25 gezeichnet wurde,
    // berechnen wir die Position basierend auf dem ersten sichtbaren Quadrat.
    // Wir addieren die halbe Breite (12.5), um den "Hotspot" in die Mitte des ersten Steins zu legen.
    double realX = itemPos.x() + (firstColWithBlock * 25) + 12.5;
    double realY = itemPos.y() + (firstRowWithBlock * 25) + 12.5;

    // Jetzt mappen wir diesen Hotspot sauber auf das 8x8 Spielfeld (cellSize=40)
    int targetCol = static_cast<int>(std::floor((realX - offsetX) / cellSize)) - firstColWithBlock;
    int targetRow = static_cast<int>(std::floor((realY - offsetY) / cellSize)) - firstRowWithBlock;

    if (board.canPlaceBlock(block, targetRow, targetCol)) {
        board.placeBlock(block, targetRow, targetCol);

        // Punkte für gesetzte Segmente berechnen
        for (const auto& row : block.getShape()) {
            for (int val : row) {
                if (val != 0) score += 10;
            }
        }

        // Volle Linien eliminieren & Bonus-Punkte addieren
        int clearedLines = board.clearFullLines();
        if (clearedLines > 0) {
            score += clearedLines * 100;
        }

        updateScoreDisplay();

        // Verbrauchten Block aufräumen
        int slot = draggedItem->getSlotIndex();
        removeItem(draggedItem);
        delete draggedItem;
        UIBlocks[slot] = nullptr;
        slotOccupied[slot] = false;

        // Spielfeld aktualisieren
        drawBoard();

        // Wenn die Auslage leer ist: 3 neue holen
        if (!slotOccupied[0] && !slotOccupied[1] && !slotOccupied[2]) {
            spawnNewBlocks();
        }

        checkGameOver();
    } else {
        // Ungültiger Zug -> Block fliegt zurück
        draggedItem->resetPosition();
    }
}
void GameScene::checkGameOver() {
    bool movePossible = false;

    for (int i = 0; i < 3; ++i) {
        if (slotOccupied[i] && UIBlocks[i]) {
            Block block = UIBlocks[i]->getBlockData();

            // Prüfen, ob das Teil noch irgendwo auf das 8x8 Feld passt
            for (int r = 0; r < 8; ++r) {
                for (int c = 0; c < 8; ++c) {
                    if (board.canPlaceBlock(block, r, c)) {
                        movePossible = true;
                        break;
                    }
                }
                if (movePossible) break;
            }
        }
        if (movePossible) break;
    }

    if (!movePossible && (slotOccupied[0] || slotOccupied[1] || slotOccupied[2])) {
        QGraphicsTextItem* gameOverText = new QGraphicsTextItem("GAME OVER");
        gameOverText->setDefaultTextColor(Qt::red);
        gameOverText->setFont(QFont("Arial", 32, QFont::Bold));
        gameOverText->setPos(70, 240);
        addItem(gameOverText);
    }
}