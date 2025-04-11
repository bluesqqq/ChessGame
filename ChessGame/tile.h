#ifndef TILE_H
#define TILE_H

#include "include/raylib-cpp.hpp"
#include "isometric.h"
#include "RenderQueue.h"
#include "Theme.h"

class Board;
class Piece;

class Tile {
    protected:
        Piece* currentPiece = nullptr;
        Piece* queuedPiece = nullptr;
        int lifetime = -1;

    public:
        Tile(int lifetime = -1);
        virtual ~Tile() {}

        virtual void draw(Theme& theme, RenderQueue& renderQueue, int x, int y, float z, bool selected, bool hide) = 0;

        /// <summary>
        /// Update called every frame
        /// </summary>
        void update();

        /// <summary>
        /// Updates the tile, called after every move
        /// </summary>
        virtual void updateState(Board& board) = 0;

        /// <summary>
        /// Gets the lifetime of the tile
        /// </summary>
        /// <returns>The lifetime of the piece</returns>
        int getLifetime();

		/// <summary>
		/// Sets the lifetime of the tile
		/// </summary>
		/// <param name="lifetime">Lifetime of the tile (in turns)</param>
		void setLifetime(int lifetime);

        /// <summary>
        /// Sets the piece on the tile
        /// </summary>
        /// <param name="piece">The piece to set</param>
        void setPiece(Piece* piece);

        /// <summary>
        /// Queues a piece to be set onto the tile
        /// </summary>
        /// <param name="piece">The piece to queue</param>
        void queuePiece(Piece* piece);

        /// <summary>
        /// Sets the queued piece onto the tile
        /// </summary>
        /// <returns>A pointer to the piece that was removed if removed, nullptr if not</returns>
        Piece* dequeuePiece();

        /// <summary>
        /// Removes the piece on the tile
        /// </summary>
        /// <returns>A pointer to the piece that was removed</returns>
        Piece* removePiece();

        /// <summary>
        /// Determines if the tile has a piece on it
        /// </summary>
        /// <returns>true if the tile has a piece, false if not</returns>
        bool hasPiece();

        /// <summary>
        /// Gets the piece on the tile
        /// </summary>
        /// <returns>A pointer to the piece on the tile</returns>
        Piece* getPiece();

        /// <summary>
        /// Determines whether this tile is selectable
        /// </summary>
        /// <returns>true if the tile is selectable, false if not</returns>
        virtual bool isSelectable();

        /// <summary>
        /// Determines if this tile can be passed by pieces that use passing (Rook, Bishop, Queen)
        /// </summary>
        /// <returns>true if the tile is passable, false if not</returns>
        virtual bool isPassable();
};

#endif