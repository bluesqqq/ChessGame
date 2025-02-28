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

    void update();

    void setTile(int row, int col, Tile* newTile);
    Tile* getTile(int row, int col);

    /// <summary>
    /// Moves a player's piece from one tile to another. Does not check if the move is valid before
    /// </summary>
    /// <param name="player">The player who is making this move</param>
    /// <param name="pieceRow">The row of the piece to move</param>
    /// <param name="pieceCol">The column of the piece to move</param>
    /// <param name="destinationRow">The row of the tile to move the piece to</param>
    /// <param name="destinationCol">The column of the tile to move the piece to</param>
    /// <returns>A pointer to the piece removed if this move results in taking a piece, nullptr if not</returns>
    Piece* movePiece(int player, int pieceRow, int pieceCol, int destinationRow, int destinationCol);

    /// <summary>
    /// Checks if the given move is valid based on a set of rules
    /// </summary>
    /// <param name="player">The player who is making this move</param>
    /// <param name="pieceRow">The row of the piece to move</param>
    /// <param name="pieceCol">The column of the piece to move</param>
    /// <param name="destinationRow">The row of the tile to move the piece to</param>
    /// <param name="destinationCol">The column of the tile to move the piece to</param>
    /// <returns>true if it is a valid move, false if not</returns>
    bool isLegalMove(int player, int pieceRow, int pieceCol, int destinationRow, int destinationCol);

    vector<pair<int, int>> getPlayersPieces(int player);

    template <typename T>
    vector<pair<int, int>> getPlayersPiecesOfType(int player);

    bool isInCheck(int player);

    bool noPossibleMoves(int player);

    bool isInCheckmate(int player);

    bool isInStalemate(int player);

};

#endif // BOARD_H
