#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <utility>
#include <iostream>
#include <optional>

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

        /// <summary>
		/// The cell of a pawn that can be captured en passant
        /// </summary>
        optional<Cell> enPassantableCell;

        int portalCounter = 0;

        void drawTile(RenderQueue& renderQueue, int rank, int file, TileType type);

    public:
        bool handlingPlayerTurn = false;
        bool handlingTileEffects = false;
        bool handlingPiecePromotion = false;
        bool handlingStateUpdate = false;

        Board(raylib::Texture2D* texture, vector<Player>& players);

        /************************************|
                 GAME LOOP FUNCTIONS
        |************************************/

        void draw(Theme& theme, RenderQueue& renderQueue, int player, Cell selectedCell);

        /// <summary>
        /// Update called every frame
        /// </summary>
        void update(int player);

        /// <summary>
        /// Updates the state of the board
        /// </summary>
        void updateState();

        /// <summary>
		/// If the board can currently be played on (make a move)
        /// </summary>
        /// <returns>true if the the board is playable, false if not</returns>
        bool isPlayable();

        /************************************|
                 PROMOTION FUNCTIONS
        |************************************/

        /// <summary>
        /// Checks the board for the specified player's pawns that need to be promoted and promotes them
        /// </summary>
        /// <param name="player">The player whos pawns to promo</param>
        void promotePieces(int player);

        /// <summary>
        /// Checks if there are any pawn promotions to handle
        /// </summary>
        /// <returns>true if there are any pawns to promote, false if not</returns>
        bool hasPromotion();

        /// <summary>
        /// Gets the cell of a pawn promotion, and removes it from the queue
        /// </summary>
        /// <returns>The cell with the pawn to be promoted</returns>
        Cell getPromotionCell();

        /************************************|
                EN PASSANT FUNCTIONS
        |************************************/

		bool hasEnPassantableCell();

		Cell getEnPassantableCell();

		void setEnPassantableCell(Cell cell);

		void clearEnPassantableCell();

        /************************************|
                   TILE FUNCTIONS
        |************************************/

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

        /// <summary>
        /// Gets a tile at a specific cell
        /// </summary>
        /// <param name="cell">The cell of the tile to get</param>
        /// <returns>A pointer to the tile at the cell</returns>
        Tile* getTile(Cell cell) {
            if (!cell.isInBounds()) return nullptr;
            return tiles[cell.rank][cell.file];
        }

        /// <summary>
        /// Gets the position of a specific tile
        /// </summary>
        /// <param name="tile">The tile to check the position of</param>
        /// <returns>A Vector2 with the tile's position if found, { -1.0f, -1.0f } if not</returns>
        raylib::Vector2 getTilePosition(Tile* tile);

        /// <summary>
        /// Gets all tiles on the board of a specific type
        /// </summary>
        /// <typeparam name="T">The type of tiles to get</typeparam>
        /// <returns>A vector of all tiles of type T</returns>
        template <typename T>
        vector<Tile*> getTilesOfType();

        /// <summary>
        /// Gets the number of tiles of a specific type
        /// </summary>
        /// <typeparam name="T">The type of tile to count</typeparam>
        /// <returns>The number of tiles of type T</returns>
        template <typename T>
        int getTileCount();

        /// <summary>
        /// Replace all expired tiles on the board with basic tiles
        /// </summary>
        void removeExpiredTiles();

        /************************************|
                   PIECE FUNCTIONS
        |************************************/

        /// <summary>
        /// Gets the piece on the specified cell
        /// </summary>
        /// <param name="cell">The cell to get the piece of</param>
        /// <returns>The piece on the cell if there is one, nullptr if not</returns>
        Piece* getPiece(Cell cell);

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
        /// Gets the cell positions of all of the specified player's pieces
        /// </summary>
        /// <param name="player">The player to get the pieces of</param>
        /// <returns>A vector of cell positions of the player's pieces</returns>
        vector<Cell> getPlayersPieces(int player);

        /// <summary>
        /// Gets the cell positions of all of the specified player's pieces of a certain type
        /// </summary>
        /// <typeparam name="T">The type of piece to look for</typeparam>
        /// <param name="player">The player to get the pieces of</param>
        /// <returns>A vector of cell positions of the player's pieces of type T</returns>
        template <typename T>
        vector<Cell> getPlayersPiecesOfType(int player);

        /************************************|
                   CELL FUNCTIONS
        |************************************/

        /// <summary>
        /// Gets the cell of the specified tile
        /// </summary>
        /// <param name="tile">The tile to get the cell of</param>
        /// <returns>The cell of the tile if found, {-1, -1} if not</returns>
        Cell getCell(Tile* tile);

        /// <summary>
        /// Gets the cell of the specified piece
        /// </summary>
        /// <param name="piece">The piece to get the cell of</param>
        /// <returns>The cell of the piece if found, {-1, -1} if not</returns>
        Cell getCell(Piece* piece);

        /// <summary>
        /// Gets the isometric position of a cell on the board
        /// </summary>
        /// <param name="cell">The cell to get the position of</param>
        /// <returns>A Vector3 of the cell's position</returns>
        raylib::Vector3 getIsoPositionAtCell(Cell cell);

        /// <summary>
        /// Gets the 2D screen position of a cell on the board
        /// </summary>
        /// <param name="cell">The cell to get the position of</param>
        /// <returns>A Vector2 of the cell's 2D screen position</returns>
        raylib::Vector2 getScreenPositionAtCell(Cell cell);

		/// <summary>
		/// Gets the cell at a specific isometric position
		/// </summary>
		/// <param name="isoPosition">The Vector3 position of the cell to get</param>
		/// <returns>The cell at isoPosition, ignoring Z axis</returns>
        Cell getCellAtIsoPosition(raylib::Vector3 isoPosition);

		/// <summary>
		/// Gets the cell at a specific screen position
		/// </summary>
		/// <param name="screenPosition">The 2D screen position of the cell</param>
		/// <param name="camera">Camera for translation (will probably remove)</param>
		/// <returns>The cell at the screen position</returns>
        Cell getCellAtScreenPosition(raylib::Vector2 screenPosition, raylib::Camera2D camera);

        /************************************|
                   MOVE FUNCTIONS
        |************************************/

        bool hasMoves() {
            return (!queuedMoves.empty());
        }

        bool allMoveAnimationsFinished() {
            for (auto& move : queuedMoves) {
                Piece* animatingPiece = getTile(move.from)->getPiece();

                if (!animatingPiece->animationFinished()) {
                    return false;
                }
            }

            return true;
        }

        /// <summary>
        /// Adds a move to the queue
        /// </summary>
        /// <param name="move">The move to add to the queue</param>
        void queueMove(Move move);

        void applyAllTileEffects() {
            cout << "Applying tile effects..." << endl;
            for (int rank = 0; rank < 8; rank++) {
                for (int file = 0; file < 8; file++) {
                    tiles[rank][file]->applyTileEffect(*this);
                }
            }
        }

        /// <summary>
        /// Removes all conflicting moves from the queue
        /// </summary>
        void removeConflictingMoves();

        /// <summary>
        /// Executes all queued moves and clears the queue
        /// </summary>
        void executeQueuedMoves();

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

        Move getMove(Cell pieceCell, Cell moveCell);

        vector<Move> getAllLegalMoves(int player);

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

        /************************************|
                 SPAWNING FUNCTIONS
        |************************************/

        void spawnRandomTiles();

        void spawnRandomTiles(TileSpawnType type);

        void printBoard() {
            cout << endl << " BOARD: " << endl;

            for (int rank = 7; rank >= 0; rank--) {
                for (int file = 0; file < 8; file++) {
					Piece* piece = tiles[rank][file]->getPiece();
                    if (piece) {
						cout << piece->getAlgebraicNotation() << " ";
                    } else {
						cout << "- ";
                    }
                }
                cout << endl;
            }
        }
};

#endif // BOARD_H
