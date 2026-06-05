#include "gameboard.h"

//Konstruktor
GameBoard::GameBoard()
{
    //ROWS und COLS constants aus header
    grid = std::vector<std::vector<int>>(ROWS, std::vector<int>(COLS, 0));
}
void GameBoard::reset()
{
    // Füllt das gesamte Spielfeld wieder mit 0 (leer)
    grid = std::vector<std::vector<int>>(ROWS, std::vector<int>(COLS, 0));
}
bool GameBoard::canPlaceBlock(const Block &block, int startRow, int startCol) const
{
    std::vector<std::vector<int>> shape = block.getShape();

    //loop über block nicht spielfeld
    for (size_t r = 0; r < shape.size(); ++r) {
        for (size_t c = 0; c < shape[r].size(); ++c) {
            //wenn an dieser stelle im block überhaupt ein Stein existiert (wert ist 1)
            if (shape[r][c] != 0) {
                //berechnet die absolute position auf dem spielfeld
                int targetRow = startRow + r;
                int targetCol = startCol + c;

                //prüfen ob der block über den spielfeldrand rausgeht
                if (targetRow < 0 || targetRow >= ROWS || targetCol < 0 || targetCol >= COLS) {
                    return false;
                }

                //prüfen ob platz schon besetzt
                if (grid[targetRow][targetCol] != 0) {
                    return false;
                }
            }
        }
    }
    return true; //keine kollisionen
}

void GameBoard::placeBlock(const Block &block, int startRow, int startCol)
{
    //kollisions und oob check
    if (!canPlaceBlock(block, startRow, startCol)) {
        return;
    }

    std::vector<std::vector<int>> shape = block.getShape();

    //block ins feld eintragen
    for (size_t r = 0; r < shape.size(); ++r) {
        for (size_t c = 0; c < shape[r].size(); ++c) {
            if (shape[r][c] != 0) {
                //placeholder 1 todo: farb id einbauen
                string color = block.getColor();
                int colorid = 1;

                if (color == "red") {
                    colorid = 1;
                } else if (color == "blue") {
                    colorid = 2;
                } else if (color == "green") {
                    colorid = 3;
                } else if (color == "yellow") {
                    colorid = 4;
                } else if (color == "orange") {
                    colorid = 5;
                }

                grid[startRow + r][startCol + c] = colorid;
            }
        }
    }
}

int GameBoard::clearFullLines()
{
    std::vector<int> volleZeilen;
    std::vector<int> volleSpalten;

    //horizontal
    for (int r = 0; r < ROWS; ++r) {
        bool istVoll = true;
        for (int c = 0; c < COLS; ++c) {
            if (grid[r][c] == 0) {
                istVoll = false;
                break;
            }
        }
        if (istVoll) {
            volleZeilen.push_back(r);
        }
    }

    //vertikal
    for (int c = 0; c < COLS; ++c) {
        bool istVoll = true;
        for (int r = 0; r < ROWS; ++r) {
            if (grid[r][c] == 0) {
                istVoll = false;
                break;
            }
        }
        if (istVoll) {
            volleSpalten.push_back(c);
        }
    }

    //volle zeile löschen
    for (int r : volleZeilen) {
        for (int c = 0; c < COLS; ++c) {
            grid[r][c] = 0;
        }
    }

    //volle spalte löschen
    for (int c : volleSpalten) {
        for (int r = 0; r < ROWS; ++r) {
            grid[r][c] = 0;
        }
    }

    //für punkte system
    return volleZeilen.size() + volleSpalten.size();
}

std::vector<std::vector<int>> GameBoard::getGrid() const
{
    return grid;
}
