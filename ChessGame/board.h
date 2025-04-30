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
#include "animation.h"
#include "RenderQueue.h"
#include "Move.h"
#include "Cell.h"

#include <queue>

using namespace std;

enum class TileSpawnType {
    ICE_SPAWN,
    CONVEYOR_ROW_SPAWN,
    CONVEYOR_LOOP_SPAWN,
    BREAK_SPAWN,
    PORTAL_SPAWN
};

class Board {
    private:
        Tile* tiles[8][8];
        raylib::Texture2D* atlas;

        vector<Player>& players;

        vector<Move> queuedMoves;

        queue<Cell> promotions;

        bool updateStatePhase = false;

        int portalCounter = 0;

        void drawTile(RenderQueue& renderQueue, int rank, int file, TileType type);

    public:
        Board(raylib::Texture2D* texture, vector<Player>& players);

        void draw(Theme& theme, RenderQueue& renderQueue, int player, Cell selectedCell);

        /// <summary>
        /// Update called every frame
        /// </summary>
        void update();

        /// <summary>
        /// Updates the state of the board
        /// </summary>
        void updateState();

        /// <summary>
        /// Replace all expired tiles on the board with basic tiles
        /// </summary>
        void removeExpiredTiles();

        /// <summary>
		/// If the board can currently be played on (make a move)
        /// </summary>
        /// <returns>true if the the board is playable, false if not</returns>
        bool isPlayable();

        /// <summary>
        /// Checks if there are any pawn promotions to handle
        /// </summary>
        /// <returns>true if there are any pawns to promote, flse if not</returns>
        bool hasPromotion() {
            return !promotions.empty();
        }

        /// <summary>
		/// Gets the cell of a pawn promotion, and removes it from the queue
        /// </summary>
        /// <returns>The cell with the pawn to be promoted</returns>
        Cell getPromotionCell() {
            if (promotions.empty()) throw std::runtime_error("promotions is empty!");

            Cell cell = promotions.front();
            promotions.pop();

            return cell;
        }

        /// <summary>
		/// Adds a move to the queue
        /// </summary>
        /// <param name="move">The move to add to the queue</param>
        void queueMove(Move move);

        /// <summary>
		/// Removes all conflicting moves from the queue
        /// </summary>
        void removeConflictingMoves();

        /// <summary>
		/// Executes all queued moves and clears the queue
        /// </summary>
        void executeQueuedMoves();

        void promotePieces() {
            // Add all promotions
            for (int file = 1; file <= 8; file++) {
                Cell cell = Cell(8, file); // Cells that a white pawn can get promoted on

                Tile* tile = getTile(cell);

                Piece* piece = tile->getPiece();

                if (piece && dynamic_cast<Pawn*>(piece) != nullptr) {
                    promotions.push(cell);
                }
            }
        }

        /// <summary>
        /// Changes a tile at a specific rank and file to a different tile
        /// </summary>
        /// <param name="rank">The rank of the tile to change</param>
        /// <param name="file">The file of the tile to change</param>
        /// <param name="newTile">The tile to change to</param>
        /// <returns>A pointer to the original tile before being changed</returns>
        Tile* setTile(int rank, int file, Tile* newTile);

        /// <summary>
        /// Changes a tile at a specific rank and file to a different tile, while also moving the piece to the new tile
        /// </summary>
        /// <param name="rank">The rank of the tile to change</param>
        /// <param name="file">The file of the tile to change</param>
        /// <param name="newTile">The tile to change to</param>
        /// <returns>A pointer to the original tile before being changed</returns>
        Tile* changeTile(int rank, int file, Tile* newTile);

        /// <summary>
        /// Gets a tile at a specific rank and file
        /// </summary>
        /// <param name="rank">The rank of the tile to get</param>
        /// <param name="file">The file of the tile to get </param>
        /// <returns>A pointer to the tile at position [rank][file]</returns>
        Tile* getTile(int rank, int file);

        Tile* getTile(Cell cell) {
            if (!cell.isInBounds()) return nullptr;
            return tiles[cell.rank - 1][cell.file - 1];
        }

        /// <summary>
        /// Gets the position of a specific tile
        /// </summary>
        /// <param name="tile">The tile to check the position of</param>
        /// <returns>A Vector2 with the tile's position if found, { -1.0f, -1.0f } if not</returns>
        raylib::Vector2 getTilePosition(Tile* tile);

        Cell getTileCell(Tile* tile) {
            for (int rank = 0; rank < 8; rank++) {
                for (int file = 0; file < 8; file++) {
                    if (tiles[rank][file] == tile) {
                        return Cell(rank + 1, file + 1);
                    }
                }
            }

            return Cell(-1, -1);
        }

        Cell getPieceCell(Piece* piece) {
            for (int rank = 0; rank < 8; rank++) {
                for (int file = 0; file < 8; file++) {
                    if (tiles[rank][file]->hasPiece() && tiles[rank][file]->getPiece() == piece) {
                        return Cell(rank + 1, file + 1);
                    }
                }
            }

            return Cell(-1, -1);
        }

        Piece* getPiece(Cell cell) {
			if (!cell.isInBounds()) return nullptr;

			Tile* tile = getTile(cell);

			if (tile) {
				return tile->getPiece();
			}

			return nullptr;
        }

        raylib::Vector2 cellToScreenPosition(Cell cell);

        raylib::Vector3 cellToIsoPosition(Cell cell);

		Cell getCellAtIsoPosition(raylib::Vector3 isoPosition) {
            return Cell(8 - isoPosition.y, isoPosition.x + 1); // Ignore Z axis 
        }

		Cell getCellAtScreenPosition(raylib::Vector2 screenPosition, raylib::Camera2D camera) {
            raylib::Vector2 translatedPosition = screenPosition - camera.offset;

            // Translate the screen position to isometric coordinates
            Vector3 isoPosition = ScreenToISO(translatedPosition, 0.0f); // NOTE: if moving the board anywhere on the z axis in the future, this will have to change
            
			return getCellAtIsoPosition(isoPosition);
		}

        /// <summary>
        /// Moves a player's piece from one tile to another. Does not check if the move is valid before
        /// </summary>
        /// <param name="player">The player who is making this move</param>
        /// <param name="pieceRow">The rank of the piece to move</param>
        /// <param name="pieceCol">The file of the piece to move</param>
        /// <param name="destinationRow">The rank of the tile to move the piece to</param>
        /// <param name="destinationCol">The file of the tile to move the piece to</param>
        /// <returns>A pointer to the piece removed if this move results in taking a piece, nullptr if not</returns>
        Piece* movePiece(int player, Cell piece, Cell move);

        /// <summary>
        /// Checks if the given move is valid based on a set of rules
        /// </summary>
        /// <param name="player">The player who is making this move</param>
        /// <param name="pieceRow">The rank of the piece to move</param>
        /// <param name="pieceCol">The file of the piece to move</param>
        /// <param name="destinationRow">The rank of the tile to move the piece to</param>
        /// <param name="destinationCol">The file of the tile to move the piece to</param>
        /// <returns>true if it is a valid move, false if not</returns>
        bool isLegalMove(int player, Cell piece, Cell move);

        vector<Move> getAllLegalMoves(int player);

        std::vector<Cell> getPlayersPieces(int player);

        template <typename T>
        std::vector<Cell> getPlayersPiecesOfType(int player);

        template <typename T>
        vector<Tile*> getTilesOfType();

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

        void spawnRandomTiles();

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
