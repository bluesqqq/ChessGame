#include "textures.h"

Rectangle SpriteBounds::toSpriteRect() const {
    return { (float)tileX * TILE_SIZE, (float)tileY * TILE_SIZE, (float)tileWidth * TILE_SIZE, (float)tileHeight * TILE_SIZE };
}

SpriteBounds pieceSprites[] = {
    {4, 0, 1, 1}, // Knight
    {4, 1, 1, 1}, // Pawn
    {5, 0, 1, 1}, // Bishop
    {5, 1, 1, 1}, // Rook
    {6, 0, 1, 2}, // Queen
    {7, 0, 1, 2}, // King

    {0, 4, 1, 2}, // Pawn   (ice variant)
    {1, 4, 1, 2}, // Knight (ice variant)
    {2, 4, 1, 2}, // Bishop (ice variant)
    {3, 4, 1, 2}, // Rook   (ice variant)
    {4, 4, 1, 2}, // Queen  (ice variant)
    {5, 4, 1, 2}, // King   (ice variant)
};

TilePosition tileData[] = {
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
    {3, 2},  // TILE_NE_CORNER
    {4, 2},  // TILE_HORIZONTAL_CONVEYOR
    {5, 2}  // TILE_VERTICAL_CONVEYOR
};