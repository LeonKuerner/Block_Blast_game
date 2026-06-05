#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include "block.h"
#include <vector>

class GameBoard
{
private:
    //Spielfeld (0 -> leerer platz !0 -> farbe)
    std::vector<std::vector<int>> grid;

    const int ROWS = 8;
    const int COLS = 8;

public:
    GameBoard();

    bool canPlaceBlock(const Block &block, int startRow, int startCol) const;

    void placeBlock(const Block &block, int startRow, int startCol);

    //Zeilen und Spalten
    int clearFullLines();
    void reset();

    std::vector<std::vector<int>> getGrid() const;
};

#endif // GAMEBOARD_H
