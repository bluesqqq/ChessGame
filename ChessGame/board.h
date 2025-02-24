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

    vector<pair<int, int>> getPlayersPieces(int player);

    template <typename T>
    vector<pair<int, int>> getPlayersPiecesOfType(int player);

    bool isInCheck(int player);

    bool noPossibleMoves(int player);

    bool isInCheckmate(int player);

    bool isInStalemate(int player);

};

#endif // BOARD_H
