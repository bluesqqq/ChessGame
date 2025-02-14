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
    int currentTurn = 0;

public:
    Board(Texture2D* texture);

    void draw(int x, int y);

    void SetTile(int row, int col, Tile* newTile);
    Tile* GetTile(int row, int col);

    bool MovePiece(int pieceRow, int pieceCol, int destinationRow, int destinationCol);

    int GetPlayerTurn();
};

#endif // BOARD_H
