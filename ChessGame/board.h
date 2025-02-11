#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <utility>

#include "tile.h"
#include "piece.h"
#include "include/raylib-cpp.hpp"

class Board
{
private:
    Tile* tiles[8][8];
    Texture2D* atlas;

public:
    Board(Texture2D* texture);

    void draw(int x, int y);

    void SetTile(int row, int col, Tile* newTile);
    Tile* GetTile(int row, int col);
};

#endif // BOARD_H
