
#ifndef TEXTURES_H
#define TEXTURES_H

#include "config.h"
#include "raylib.h"

typedef struct {
    int tileX; // Column in tileset
    int tileY; // Row in tileset
} TilePosition;

struct SpriteBounds {
    int tileX; // Top of sprite
    int tileY; // Left of sprite
    int tileWidth; // Width (in tiles) of sprite
    int tileHeight; // Height (in tiles) of sprite

    Rectangle toSpriteRect() const;
};

typedef enum {
    TILE_WHITE_CUBE,
    TILE_BLACK_CUBE,
    TILE_RED_CUBE,
    TILE_ICE,
    TILE_BREAK_SMALL,
    TILE_BREAK_MEDIUM,
    TILE_BREAK_LARGE,
    TILE_PORTAL,
    TILE_GRASS_LIGHT,
    TILE_GRASS_DARK,
    TILE_EAST_WALL,
    TILE_SOUTH_WALL,
    TILE_NW_CORNER,
    TILE_SW_CORNER,
    TILE_NORTH_WALL,
    TILE_WEST_WALL,
    TILE_SE_CORNER,
    TILE_NE_CORNER,
    TILE_HORIZONTAL_CONVEYOR,
    TILE_VERTICAL_CONVEYOR,
    TILE_COUNT // Total number of tiles
} TileType;

typedef enum {
    SPRITE_KNIGHT,
    SPRITE_PAWN,
    SPRITE_BISHOP,
    SPRITE_ROOK,
    SPRITE_QUEEN,
    SPRITE_KING,

    SPRITE_PAWN_FROZEN,
    SPRITE_KNIGHT_FROZEN,
    SPRITE_BISHOP_FROZEN,
    SPRITE_ROOK_FROZEN,
    SPRITE_QUEEN_FROZEN,
    SPRITE_KING_FROZEN
} PieceType;

extern SpriteBounds pieceSprites[];
extern TilePosition tileData[];

#endif