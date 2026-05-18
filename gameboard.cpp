#include "gameboard.h"

// Konstruktor: Initialisiert das 8x8 Spielfeld komplett mit Nullen (0 = leer)
GameBoard::GameBoard()
{
    // ROWS und COLS sind in gameboard.h als 8 definiert
    grid = std::vector<std::vector<int>>(ROWS, std::vector<int>(COLS, 0));
}

// Prüft, ob ein Block an einer bestimmten Position platziert werden DARF
bool GameBoard::canPlaceBlock(const Block& block, int startRow, int startCol) const {
    // Wir holen uns die Form des Blocks (z.B. eine 3x3 Matrix)
    std::vector<std::vector<int>> shape = block.getShape();

    // Wir loopen NUR über die Zeilen und Spalten des BLOCKS, nicht des Spielfelds!
    for (size_t r = 0; r < shape.size(); ++r) {
        for (size_t c = 0; c < shape[r].size(); ++c) {

            // Wenn an dieser Stelle im Block überhaupt ein Stein existiert (Wert ist 1)
            if (shape[r][c] != 0) {
                // Berechne die absolute Position auf dem Spielfeld
                int targetRow = startRow + r;
                int targetCol = startCol + c;

                // 1. Prüfen, ob der Block über den Spielfeldrand hinausragt
                if (targetRow < 0 || targetRow >= ROWS || targetCol < 0 || targetCol >= COLS) {
                    return false; // Passt nicht, bricht sofort ab
                }

                // 2. Prüfen, ob der Platz auf dem Spielfeld bereits besetzt ist (!= 0)
                if (grid[targetRow][targetCol] != 0) {
                    return false; // Kollision, bricht sofort ab
                }
            }
        }
    }
    return true; // Keine Kollisionen gefunden: Der Block passt!
}

// Platziert den Block permanent auf dem Spielfeld
void GameBoard::placeBlock(const Block& block, int startRow, int startCol) {
    // Sicherheitsprüfung: Nur platzieren, wenn es auch wirklich erlaubt ist
    if (!canPlaceBlock(block, startRow, startCol)) {
        return;
    }

    std::vector<std::vector<int>> shape = block.getShape();

    // Trage die Blockteile in das Spielfeld-Grid ein
    for (size_t r = 0; r < shape.size(); ++r) {
        for (size_t c = 0; c < shape[r].size(); ++c) {
            if (shape[r][c] != 0) {
                // Hier tragen wir z.B. eine 1 ein, um zu zeigen: Besetzt!
                // Später kannst du hier auch Farb-IDs eintragen.
                grid[startRow + r][startCol + c] = 1;
            }
        }
    }
}

// Findet volle Zeilen/Spalten, löscht sie und gibt die Anzahl zurück
int GameBoard::clearFullLines() {
    std::vector<int> volleZeilen;
    std::vector<int> volleSpalten;

    // 1. Horizontale Zeilen prüfen
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

    // 2. Vertikale Spalten prüfen
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

    // 3. Volle Zeilen löschen
    for (int r : volleZeilen) {
        for (int c = 0; c < COLS; ++c) {
            grid[r][c] = 0;
        }
    }

    // 4. Volle Spalten löschen
    for (int c : volleSpalten) {
        for (int r = 0; r < ROWS; ++r) {
            grid[r][c] = 0;
        }
    }

    // Gibt die Summe der gelöschten Linien zurück (wichtig für die Punkte!)
    return volleZeilen.size() + volleSpalten.size();
}

// Gibt den aktuellen Zustand des Spielfelds zurück (für die GUI)
std::vector<std::vector<int>> GameBoard::getGrid() const {
    return grid;
}