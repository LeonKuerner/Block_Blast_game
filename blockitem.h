#ifndef BLOCKITEM_H
#define BLOCKITEM_H

#include <QGraphicsItemGroup>
#include <QGraphicsSceneMouseEvent>
#include <QColor>
#include "block.h"

class BlockItem : public QGraphicsItemGroup
{
private:
    Block blockData;
    QPointF originalPosition;
    bool isDragging;
    int slotIndex; // 0, 1 oder 2 für die Auslage unten

    QColor getColorFromString(const std::string& colorName);

public:
    BlockItem(const Block& block, int slotIndex, QGraphicsItem* parent = nullptr);

    Block getBlockData() const;
    int getSlotIndex() const { return slotIndex; }

    void resetPosition();
    void saveCurrentPosition();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // BLOCKITEM_H