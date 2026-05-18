#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <vector>
#include "block.h" // Wir brauchen die Block-Klasse hier!

class GameBoard
{
private:
    // Das Spielfeld-Raster (0 = leer, >0 = besetzt mit einer Farb-ID)
    std::vector<std::vector<int>> grid;

    // Konstanten für die Spielfeldgröße (Block Blast nutzt meistens 8x8)
    const int ROWS = 8;
    const int COLS = 8;

public:
    GameBoard();

    // Prüft, ob ein Block an einer bestimmten Position (oben links des Blocks) platziert werden kann
    bool canPlaceBlock(const Block& block, int startRow, int startCol) const;

    // Platziert den Block permanent auf dem Spielfeld (ändert die Nullen in Farb-IDs)
    void placeBlock(const Block& block, int startRow, int startCol);

    // Prüft auf volle Zeilen UND Spalten, löscht sie und gibt die Anzahl der gelöschten Linien zurück
    int clearFullLines();

    // Getter, damit die GUI später weiß, was sie zeichnen muss
    std::vector<std::vector<int>> getGrid() const;
};

#endif // GAMEBOARD_H