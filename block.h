#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <vector>

using namespace std;

class Block
{
private:
    string color;

    //Art des blockes
    vector<vector<int>> shape;

    int posX;
    int posY;

public:
    //Konstruktor
    Block(string color, vector<vector<int>> shape);

    void setPosition(int x, int y);
    int getX() const;
    int getY() const;

    string getColor() const;
    void setColor(string newColor);

    vector<vector<int>> getShape() const;
};

#endif // BLOCK_H