#include "customtiles.h"
#include "piece.h"
#include "textures.h"
#include "board.h"
#include <iostream>

BasicTile::BasicTile(Texture2D* texture) : Tile(), atlas(texture) {}

void BasicTile::draw(int x, int y, float z, bool selected, bool hide) {
    TileType tileType = ((x + y) % 2 == 0) ? TILE_WHITE_CUBE : TILE_BLACK_CUBE;

    TilePosition tile = tileData[tileType];

    Rectangle source = {
        tile.tileX * TILE_SIZE, tile.tileY * TILE_SIZE,
        TILE_SIZE, TILE_SIZE
    };

    Vector2 position = IsoToScreen(x, y, z);

    DrawTextureRec(*atlas, source, position, selected ? RED : WHITE);

    if (currentPiece != nullptr) {
        currentPiece->draw(x, y, z, hide);
    }
}

void BasicTile::update(Board& board) {
    // Update the piece on this tile
    if (hasPiece()) {
        currentPiece->update();
    }
}

bool BasicTile::isSelectable()
{
    return true; // Basic tile will always be selectable
}

IceTile::IceTile(Texture2D* texture) : Tile(1), atlas(texture) {}

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

void IceTile::update(Board& board) {
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

        currentPiece->setFrozen(6);
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

    Rectangle source = { tile.tileX * TILE_SIZE, tile.tileY * TILE_SIZE, TILE_SIZE, TILE_SIZE };

    Vector2 position = IsoToScreen(x, y, z);

    DrawTextureRec(*atlas, source, position, selected ? RED : BROWN);

    if (currentPiece != nullptr)
    {
        currentPiece->draw(x, y, z, hide);
    }
}

void BreakingTile::update(Board& board) {
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



ConveyorTile::ConveyorTile(Texture2D* texture, Direction direction) : Tile(10), atlas(texture), direction(direction) {}

void ConveyorTile::draw(int x, int y, float z, bool selected, bool hide) {

    TileType tileType = TILE_HORIZONTAL_CONVEYOR;

    if (direction == UP || direction == DOWN) {
        tileType = TILE_VERTICAL_CONVEYOR;
    }

    TilePosition tile = tileData[tileType];

    Rectangle source = { tile.tileX * TILE_SIZE, tile.tileY * TILE_SIZE, TILE_SIZE, TILE_SIZE };

    Vector2 position = IsoToScreen(x, y, z);

    DrawTextureRec(*atlas, source, position, selected ? RED : BROWN);

    if (currentPiece != nullptr) {
        currentPiece->draw(x, y, z, hide);
    }
}

void ConveyorTile::update(Board& board) {
    // This needs to be fixed, if the updates are called in a specific order
    // Pieces can tend to "glide" all the way to the end of a conveyor belt row
    if (hasPiece()) {
        raylib::Vector2 tilePosition = board.getTilePosition(this);

        switch (direction) {
            case UP:
                tilePosition += { 0, 1 };
                break;
            case DOWN:
                tilePosition += { 0, -1 };
                break;
            case LEFT:
                tilePosition += { -1, 0 };
                break;
            case RIGHT:
                tilePosition += { 1, 0 };
                break;
        }

        Tile* destinationTile = board.getTile(tilePosition.x, tilePosition.y);

        if (destinationTile) {
            // Queue a movement to the destination tile
            board.addQueuedMove({ destinationTile, this, false });
        }
    }

    lifetime--;
}

bool ConveyorTile::isSelectable() {
    return true;
}