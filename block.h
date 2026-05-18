#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <vector>

class Block
{
private:
    std::string color; // z.B. "red", "blue" oder als QColor in Qt later

    // Die Form des Blocks (z.B. ein 3x3 Gitter für ein L-Stück)
    // 1 steht für "Block-Teil vorhanden", 0 für "leer"
    std::vector<std::vector<int>> shape;

    // Die aktuelle X/Y-Position auf dem Spielfeld (oder der Auslage)
    int posX;
    int posY;

public:
    // Konstruktor: Übergib die Farbe und die Form (Matrix)
    Block(std::string color, std::vector<std::vector<int>> shape);

    // Getter und Setter für die Position
    void setPosition(int x, int y);
    int getX() const;
    int getY() const;

    // Getter und Setter für die Farbe
    std::string getColor() const;
    void setColor(std::string newColor);

    // Getter für die Form (wichtig für die Kollisionsabfrage)
    std::vector<std::vector<int>> getShape() const;
};

#endif // BLOCK_H