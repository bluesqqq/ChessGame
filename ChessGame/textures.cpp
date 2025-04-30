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

SpriteBounds iconSprites[] = {
    {7, 2, 1, 1}, // Knight
    {7, 3, 1, 1}, // Rook
    {7, 4, 1, 1}, // Bishop
    {7, 5, 1, 1} // Queen
};

TilePosition tileData[] = {
    {0, 0}, // TILE_WHITE_CUBE
    {1, 0}, // TILE_BLACK_CUBE
    {2, 0}, // TILE_RED_CUBE
    {6, 2}, // TILE_ICE

    {0, 3}, // TILE_BREAK_SMALL
    {1, 3}, // TILE_BREAK_MEDIUM
    {2, 3}, // TILE_BREAK_LARGE

    {3, 3}, // TILE_PORTAL
    {4, 3}, // TILE_GRASS_LIGHT
    {5, 3}, // TILE_GRASS_DARK

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
