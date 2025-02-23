#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <utility>

#include "tile.h"
#include "piece.h"
#include "player.h"
#include "include/raylib-cpp.hpp"

class Board
{
private:
    Tile* tiles[8][8];
    Texture2D* atlas;

    vector<Player>& players;

public:
    Board(Texture2D* texture, vector<Player>& players);

    void draw(int player, int x, int y);

    void setTile(int row, int col, Tile* newTile);
    Tile* getTile(int row, int col);

    bool movePiece(int player, int pieceRow, int pieceCol, int destinationRow, int destinationCol);
};

#endif // BOARD_H
