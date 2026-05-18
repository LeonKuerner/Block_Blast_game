#include "blockitem.h"
#include <QGraphicsRectItem>
#include <QBrush>
#include <QPen>
#include "gamescene.h"

BlockItem::BlockItem(const Block& block, int slotIndex, QGraphicsItem* parent)
    : QGraphicsItemGroup(parent), blockData(block), isDragging(false), slotIndex(slotIndex)
{
    auto shape = blockData.getShape();
    const int cellSize = 25; // In der Auslage etwas kompakter

    QColor blockColor = getColorFromString(blockData.getColor());

    for (size_t r = 0; r < shape.size(); ++r) {
        for (size_t c = 0; c < shape[r].size(); ++c) {
            if (shape[r][c] != 0) {
                QGraphicsRectItem* rect = new QGraphicsRectItem(c * cellSize, r * cellSize, cellSize, cellSize);
                rect->setBrush(QBrush(blockColor));
                rect->setPen(QPen(Qt::white, 1));
                addToGroup(rect);
            }
        }
    }

    setFlag(QGraphicsItem::ItemIsMovable, true);
}

QColor BlockItem::getColorFromString(const std::string& colorName) {
    if (colorName == "red") return QColor(230, 50, 50);
    if (colorName == "blue") return QColor(0, 120, 215);
    if (colorName == "green") return QColor(50, 180, 50);
    if (colorName == "yellow") return QColor(240, 200, 0);
    if (colorName == "orange") return QColor(255, 130, 0);
    return QColor(128, 128, 128); // Fallback-Grau
}

Block BlockItem::getBlockData() const { return blockData; }
void BlockItem::saveCurrentPosition() { originalPosition = pos(); }
void BlockItem::resetPosition() { setPos(originalPosition); }

void BlockItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    isDragging = true;
    setScale(1.2); // Vergrößern beim Ziehen
    setZValue(10); // Immer im Vordergrund zeichnen
    QGraphicsItemGroup::mousePressEvent(event);
}

void BlockItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (isDragging) {
        QGraphicsItemGroup::mouseMoveEvent(event);
    }
}

void BlockItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    isDragging = false;
    setScale(1.0);
    setZValue(0);
    QGraphicsItemGroup::mouseReleaseEvent(event);

    // Wir holen uns die Szene, in der dieser Block liegt
    QGraphicsScene* currentScene = scene();
    if (currentScene) {
        // Wir casten die Standard-Szene zu unserer spezifischen GameScene
        GameScene* gameScene = dynamic_cast<GameScene*>(currentScene);
        if (gameScene) {
            // Wir übergeben das gesamte BlockItem-Objekt an die Szene zur Auswertung
            gameScene->handleBlockPlacement(this);
        }
    }
}