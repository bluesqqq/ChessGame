#include "tile.h"
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

void Tile::draw(int x, int y, float z, bool selected, bool hide)
{
    // Empty base class function to be overridden
}

void Tile::setPiece(Piece* piece)
{
    currentPiece = piece;
}

Piece* Tile::removePiece()
{
    Piece* tempPiece = currentPiece;
    currentPiece = nullptr;
    return tempPiece;
}

bool Tile::hasPiece()
{
    return currentPiece != nullptr;
}

Piece* Tile::getPiece()
{
    return currentPiece;
}

bool Tile::isSelectable()
{
    return true; // Base tile is always selectable
}

BasicTile::BasicTile(Texture2D* texture) : atlas(texture) {}

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

bool BasicTile::isSelectable()
{
    return true; // Basic tile will always be selectable
}
