# Block Blast Game

Ein kleines Block-Blast-Spiel, entwickelt mit C++ und Qt.

## Voraussetzungen

Folgende Software muss installiert sein:

* CMake
* GNU C++ Compiler (g++)
* Qt 6 Widgets

### Ubuntu

```bash
sudo apt update
sudo apt install cmake g++ make qt6-base-dev qt6-tools-dev
```

## Repository klonen

```bash
git clone <repository-url>
cd Block_Blast_game
```

## Projekt bauen

Build-Ordner erstellen und Projekt kompilieren:

```bash
cmake -S . -B ../Block_Blast_build
cmake --build ../Block_Blast_build
```

## Spiel starten

### Linux (X11)

```bash
QT_QPA_PLATFORM=xcb ../Block_Blast_build/Block_Blast
```

### Wayland

```bash
../Block_Blast_build/Block_Blast
```

## Features

* Hauptmenü
* Drag-and-Drop Spielmechanik
* Punktesystem
* Restart-Funktion
* Game-Over-Erkennung
* Reihen löschen
* Spezialeffekte beim Löschen von Reihen

## Projektstruktur

```text
Block_Blast_game/
├── main.cpp
├── menuwindow.cpp
├── menuwindow.h
├── gamescene.cpp
├── gamescene.h
├── gameboard.cpp
├── gameboard.h
├── block.cpp
├── block.h
├── blockitem.cpp
├── blockitem.h
└── CMakeLists.txt
```

