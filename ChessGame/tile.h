#pragma once
#include "raylib.h"
#include "isometric.h"
#include "piece.h"

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


sTile tileData[] = {
    {0, 0}, // TILE_WHITE_CUBE
    {1, 0}, // TILE_BLACK_CUBE
    {2, 0}, // TILE_BLACK_CUBE
    {0, 1}, // TILE_EAST_WALL
    {1, 1}, // TILE_SOUTH_WALL
    {2, 1}, // TILE_NW_CORNER
    {3, 1}, // TILE_SW_CORNER
    {0, 2}, // TILE_NORTH_WALL
    {1, 2}, // TILE_WEST_WALL
    {2, 2}, // TILE_SE_CORNER
    {3, 2}  // TILE_NE_CORNER
};

class Tile
{
protected:
    Piece* currentPiece = nullptr;

public:
    Tile() {}
    virtual ~Tile() {}

    virtual void draw(int x, int y, float z) {

    }

    /// @brief Sets this tile's piece to the given piece.
    /// @param piece Pointer to the piece to set.
    void setPiece(Piece* piece)
    {
        currentPiece = piece;
    }

    /// @brief Removes the piece from this tile.
    /// @return The piece that was removed.
    Piece* removePiece()
    {
        Piece* tempPiece = currentPiece;
        currentPiece = nullptr;

        return tempPiece;
    }

    /// @brief If the board has a piece currently on it or not.
    /// @return `true` is there is a piece, `false` if not.
    bool hasPiece()
    {
        if (currentPiece == nullptr) return false;

        return true;
    }

    /// @brief If the current tile can be selected by the player
    /// @return `true` if the tile is selectable, `false` if not
    virtual bool isSelectable()
    {
        return true;
    }
};

class BasicTile : public Tile
{
private:
    Texture2D* atlas;

public:
    BasicTile(Texture2D* texture) : atlas(texture)
    {

    }

    void draw(int x, int y, float z) override
    {
        TileType tileType = ((x + y) % 2 == 0) ? TILE_WHITE_CUBE : TILE_BLACK_CUBE;

        sTile tile = tileData[tileType];

        Rectangle source = {
            tile.tileX * TILE_SIZE, tile.tileY * TILE_SIZE,
            TILE_SIZE, TILE_SIZE
        };

        Vector2 position = IsoToScreen(x, y, z);

        DrawTextureRec(*atlas, source, position, WHITE);

        if (currentPiece != nullptr)
        {
            currentPiece->draw(x, y, z);
        }
    }

    bool isSelectable() override
    {
        // Basic tile will always be selectable
        return true;
    }
};
