#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <utility>
#include <iostream>

#include "tile.h"
#include "customtiles.h"
#include "piece.h"
#include "player.h"
#include "include/raylib-cpp.hpp"
#include "textures.h"

enum class TileSpawnType {
    ICE_SPAWN,
    CONVEYOR_ROW_SPAWN,
    CONVEYOR_LOOP_SPAWN,
    BREAK_SPAWN
};

struct Move {
    Tile* to;
    Tile* from;
    bool canOvertake;
};

class Board {
    private:
        Tile* tiles[8][8];
        Texture2D* atlas;

        vector<Player>& players;

        vector<Move> queuedMoves;

        void drawTile(int row, int col, TileType type);

    public:
        Board(Texture2D* texture, vector<Player>& players);

        void draw(int player, int x, int y);

        void update();

        void addQueuedMove(Move move);

        void executeQueuedMoves();

        /// <summary>
        /// Changes a tile at a specific row and column to a different tile
        /// </summary>
        /// <param name="row">The row of the tile to change</param>
        /// <param name="col">The column of the tile to change</param>
        /// <param name="newTile">The tile to change to</param>
        /// <returns>A pointer to the original tile before being changed</returns>
        Tile* setTile(int row, int col, Tile* newTile);

        /// <summary>
        /// Changes a tile at a specific row and column to a different tile, while also moving the piece to the new tile
        /// </summary>
        /// <param name="row">The row of the tile to change</param>
        /// <param name="col">The column of the tile to change</param>
        /// <param name="newTile">The tile to change to</param>
        /// <returns>A pointer to the original tile before being changed</returns>
        Tile* changeTile(int row, int col, Tile* newTile);

        /// <summary>
        /// Gets a tile at a specific row and column
        /// </summary>
        /// <param name="row">The row of the tile to get</param>
        /// <param name="col">The column of the tile to get </param>
        /// <returns>A pointer to the tile at position [row][col]</returns>
        Tile* getTile(int row, int col);

        /// <summary>
        /// Gets the position of a specific tile
        /// </summary>
        /// <param name="tile">The tile to check the position of</param>
        /// <returns>A Vector2 with the tile's position if found, { -1.0f, -1.0f } if not</returns>
        raylib::Vector2 getTilePosition(Tile* tile);

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

        template <typename T>
        vector<pair<int, int>> getTilesOfType();

        /// <summary>
        /// Determines if a player is in check
        /// </summary>
        /// <param name="player">The player to check for check</param>
        /// <returns>true is player is in check, false if not</returns>
        bool isInCheck(int player);

        /// <summary>
        /// Determines if a player has any possible moves
        /// </summary>
        /// <param name="player">The player to check for possible moves</param>
        /// <returns>true is player has any moves, false if not</returns>
        bool canMove(int player);

        /// <summary>
        /// Determines if a player is in checkmate
        /// </summary>
        /// <param name="player">The player to check for check</param>
        /// <returns>true is player is in checkmate, false if not</returns>
        bool isInCheckmate(int player);

        /// <summary>
        /// Determines if a player is in a stalemate
        /// </summary>
        /// <param name="player">The player to check for stalemate</param>
        /// <returns>true is player is in stalemate, false if not</returns>
        bool isInStalemate(int player);

        void spawnRandomTiles(TileSpawnType type);

        /// <summary>
        /// Gets the number of tiles of a specific type
        /// </summary>
        /// <typeparam name="T">The type of tile to count</typeparam>
        /// <returns>The number of tiles of type T</returns>
        template <typename T>
        int getTileCount();
};

#endif // BOARD_H
