#include "customtiles.h"
#include "piece.h"

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

BasicTile::BasicTile(Texture2D* texture) : Tile(), atlas(texture) {}

void BasicTile::draw(int x, int y, float z, bool selected, bool hide)
{
    TileType tileType = ((x + y) % 2 == 0) ? TILE_WHITE_CUBE : TILE_BLACK_CUBE;

    sTile tile = tileData[tileType];

    Rectangle source = {
        tile.tileX * TILE_SIZE, tile.tileY * TILE_SIZE,
        TILE_SIZE, TILE_SIZE
    };

    Vector2 position = IsoToScreen(x, y, z);

    DrawTextureRec(*atlas, source, position, selected ? RED : WHITE);

    if (currentPiece != nullptr)
    {
        currentPiece->draw(x, y, z, hide);
    }
}

void BasicTile::update() {
    // Update the piece on this tile
    if (hasPiece()) {
        currentPiece->setImmobile(false);
        currentPiece->update();
    }
}

bool BasicTile::isSelectable()
{
    return true; // Basic tile will always be selectable
}

IceTile::IceTile(Texture2D* texture) : Tile(4), atlas(texture) {}

void IceTile::draw(int x, int y, float z, bool selected, bool hide)
{
    TileType tileType = ((x + y) % 2 == 0) ? TILE_WHITE_CUBE : TILE_BLACK_CUBE;

    sTile tile = tileData[tileType];

    Rectangle source = {
        tile.tileX * TILE_SIZE, tile.tileY * TILE_SIZE,
        TILE_SIZE, TILE_SIZE
    };

    Vector2 position = IsoToScreen(x, y, z);

    DrawTextureRec(*atlas, source, position, selected ? RED : BLUE);

    if (currentPiece != nullptr)
    {
        currentPiece->draw(x, y, z, hide);
    }
}

void IceTile::update() {
    // Update the piece on this tile
    if (hasPiece()) {
        currentPiece->setImmobile(true);
        currentPiece->update();
        lifetime--;
    }
}

bool IceTile::isSelectable()
{
    return true; // Basic tile will always be selectable
}

BreakingTile::BreakingTile(Texture2D* texture) : Tile(6), atlas(texture) {}

void BreakingTile::draw(int x, int y, float z, bool selected, bool hide)
{
    TileType tileType = ((x + y) % 2 == 0) ? TILE_WHITE_CUBE : TILE_BLACK_CUBE;

    sTile tile = tileData[tileType];

    Rectangle source = {
        tile.tileX * TILE_SIZE, tile.tileY * TILE_SIZE,
        TILE_SIZE, TILE_SIZE
    };

    Vector2 position = IsoToScreen(x, y, z);

    DrawTextureRec(*atlas, source, position, selected ? RED : BROWN);

    if (currentPiece != nullptr)
    {
        currentPiece->draw(x, y, z, hide);
    }
}

void BreakingTile::update() {
    // Update the piece on this tile
    if (hasPiece()) {
        lifetime--;
        if (!lifetime) {
            removePiece();
        }
    }
}

bool BreakingTile::isSelectable()
{
    if (lifetime) {
        return true; // Basic tile will always be selectable
    }
    else {
        return false;
    }
}