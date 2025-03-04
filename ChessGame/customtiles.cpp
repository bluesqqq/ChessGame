#include "customtiles.h"
#include "piece.h"
#include "textures.h"

BasicTile::BasicTile(Texture2D* texture) : Tile(), atlas(texture) {}

void BasicTile::draw(int x, int y, float z, bool selected, bool hide)
{
    TileType tileType = ((x + y) % 2 == 0) ? TILE_WHITE_CUBE : TILE_BLACK_CUBE;

    TilePosition tile = tileData[tileType];

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
        currentPiece->setFrozen(false);
        currentPiece->update();
    }
}

bool BasicTile::isSelectable()
{
    return true; // Basic tile will always be selectable
}

IceTile::IceTile(Texture2D* texture) : Tile(6), atlas(texture) {}

void IceTile::draw(int x, int y, float z, bool selected, bool hide)
{
    TileType tileType = ((x + y) % 2 == 0) ? TILE_WHITE_CUBE : TILE_BLACK_CUBE;

    TilePosition tile = tileData[tileType];

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

        switch (lifetime) {
            case 6: {
                Sound fxThaw = LoadSound("resources/icefrozen.wav");
                PlaySound(fxThaw);
                break;
            }
            case 4: {
                Sound fxThaw = LoadSound("resources/icethaw1.wav");
                PlaySound(fxThaw);
                break;
            }
            case 2: {
                Sound fxThaw = LoadSound("resources/icethaw2.wav");
                PlaySound(fxThaw);
                break;
            }
            case 0: {
                Sound fxThaw = LoadSound("resources/icebreak.wav");
                PlaySound(fxThaw);
                break;
            }
        }

        currentPiece->setFrozen(true);
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

    TilePosition tile = tileData[tileType];

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