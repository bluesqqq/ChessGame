#pragma once
#ifndef TILE_H
#define TILE_H

#include "include/raylib-cpp.hpp"
#include "isometric.h"

class Board;
class Piece;

class Tile
{
    protected:
        Piece* currentPiece = nullptr;
        Piece* queuedPiece = nullptr;
        int lifetime = -1;

    public:
        Tile(int lifetime = -1);
        virtual ~Tile() {}

        virtual void draw(int x, int y, float z, bool selected, bool hide) = 0;

        /// <summary>
        /// Updates the tile, called after every move
        /// </summary>
        virtual void update(Board& board) = 0;

        int getLifetime();

        void setPiece(Piece* piece);
        void queuePiece(Piece* piece);
        Piece* dequeuePiece();
        Piece* removePiece();
        bool hasPiece();
        Piece* getPiece();
        virtual bool isSelectable();
};

#endif