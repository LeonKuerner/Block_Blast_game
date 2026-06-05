#include "block.h"

Block::Block(std::string color, std::vector<std::vector<int>> shape)
    : color(color)
    , shape(shape)
    , posX(0)
    , posY(0)
{}

void Block::setPosition(int x, int y)
{
    posX = x;
    posY = y;
}

int Block::getX() const
{
    return posX;
}

int Block::getY() const
{
    return posY;
}

std::string Block::getColor() const
{
    return color;
}

void Block::setColor(std::string newColor)
{
    color = newColor;
}

std::vector<std::vector<int>> Block::getShape() const
{
    return shape;
}
