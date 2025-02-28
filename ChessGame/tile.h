#pragma once
#ifndef TILE_H
#define TILE_H

#include "include/raylib-cpp.hpp"
#include "isometric.h"

class Piece;

typedef struct {
    int tileX; // Column in tileset
    int tileY; // Row in tileset
} sTile;

typedef enum {
    TILE_WHITE_CUBE,
    TILE_BLACK_CUBE,
    TILE_RED_CUBE,
    TILE_EAST_WALL,
    TILE_SOUTH_WALL,
    TILE_NW_CORNER,
    TILE_SW_CORNER,
    TILE_NORTH_WALL,
    TILE_WEST_WALL,
    TILE_SE_CORNER,
    TILE_NE_CORNER,
    TILE_COUNT // Total number of tiles
} TileType;

class Tile
{
    protected:
        Piece* currentPiece = nullptr;
        int lifetime = -1;

    public:
        Tile() {}
        virtual ~Tile() {}

        virtual void draw(int x, int y, float z, bool selected, bool hide) = 0;

        /// <summary>
        /// Updates the tile, called after every move
        /// </summary>
        virtual void update() = 0;

        int getLifetime();

        void setPiece(Piece* piece);
        Piece* removePiece();
        bool hasPiece();
        Piece* getPiece();
        virtual bool isSelectable();
};

class BasicTile : public Tile
{
    private:
        Texture2D* atlas;

    public:
        BasicTile(Texture2D* texture);

        void draw(int x, int y, float z, bool selected, bool hide) override;

        void update() override;
        bool isSelectable() override;
};

class IceTile : public Tile
{
    private:
        Texture2D* atlas;

    public:
        IceTile(Texture2D* texture);

        void draw(int x, int y, float z, bool selected, bool hide) override;

        void update() override;
        bool isSelectable() override;
};

#endif