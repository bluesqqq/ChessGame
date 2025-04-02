#include "customtiles.h"
#include "piece.h"
#include "textures.h"
#include "board.h"
#include <iostream>

BasicTile::BasicTile(raylib::Texture2D* texture) : Tile(), atlas(texture) {}

void BasicTile::draw(RenderQueue& renderQueue, int x, int y, float z, bool selected, bool hide) {
    TileType tileType = ((x + y) % 2 == 0) ? TILE_WHITE_CUBE : TILE_BLACK_CUBE;

    TilePosition tile = tileData[tileType];

    Rectangle source = {
        tile.tileX * TILE_SIZE, tile.tileY * TILE_SIZE,
        TILE_SIZE, TILE_SIZE
    };

    renderQueue.addSpriteObject(SpriteObject(raylib::Vector3(x, y, z - 1), atlas, source, selected ? RED : WHITE));

    if (currentPiece != nullptr) {
        currentPiece->draw(renderQueue, x, y, z, hide);
    }
}

void BasicTile::updateState(Board& board) {
    // Update the piece on this tile
    if (hasPiece()) {
        currentPiece->updateState();
    }
}



IceTile::IceTile(raylib::Texture2D* texture) : Tile(1), atlas(texture) {}

void IceTile::draw(RenderQueue& renderQueue, int x, int y, float z, bool selected, bool hide) {
    TileType tileType = ((x + y) % 2 == 0) ? TILE_WHITE_CUBE : TILE_BLACK_CUBE;

    TilePosition tile = tileData[tileType];

    Rectangle source = {
        tile.tileX * TILE_SIZE, tile.tileY * TILE_SIZE,
        TILE_SIZE, TILE_SIZE
    };

    renderQueue.addSpriteObject(SpriteObject(raylib::Vector3(x, y, z - 1), atlas, source, selected ? RED : BLUE));

    if (currentPiece != nullptr) {
        currentPiece->draw(renderQueue, x, y, z, hide);
    }
}

void IceTile::updateState(Board& board) {
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
        currentPiece->updateState();

        lifetime--;
    }
}



BreakingTile::BreakingTile(raylib::Texture2D* texture) : Tile(6), atlas(texture) {}

void BreakingTile::draw(RenderQueue& renderQueue, int x, int y, float z, bool selected, bool hide) {
    TileType tileType = ((x + y) % 2 == 0) ? TILE_WHITE_CUBE : TILE_BLACK_CUBE;

    TilePosition tile = tileData[tileType];

    Rectangle source = { tile.tileX * TILE_SIZE, tile.tileY * TILE_SIZE, TILE_SIZE, TILE_SIZE };

    renderQueue.addSpriteObject(SpriteObject(raylib::Vector3(x, y, z - 1), atlas, source, selected ? RED : BROWN));

    if (currentPiece != nullptr)
    {
        currentPiece->draw(renderQueue, x, y, z, hide);
    }
}

void BreakingTile::updateState(Board& board) {
    // Update the piece on this tile
    if (hasPiece()) {
        lifetime--;
        if (!lifetime) {
            removePiece();
        }
    }
}



ConveyorTile::ConveyorTile(raylib::Texture2D* texture, Direction direction) : Tile(10), atlas(texture), direction(direction) {}

void ConveyorTile::draw(RenderQueue& renderQueue, int x, int y, float z, bool selected, bool hide) {
    TileType tileType = TILE_HORIZONTAL_CONVEYOR;

    if (direction == UP || direction == DOWN) {
        tileType = TILE_VERTICAL_CONVEYOR;
    }

    TilePosition tile = tileData[tileType];

    Rectangle source = { tile.tileX * TILE_SIZE, tile.tileY * TILE_SIZE, TILE_SIZE, TILE_SIZE };

    renderQueue.addSpriteObject(SpriteObject(raylib::Vector3(x, y, z - 1), atlas, source, selected ? RED : WHITE));

    if (currentPiece != nullptr) {
        currentPiece->draw(renderQueue, x, y, z, hide);
    }
}

void ConveyorTile::updateState(Board& board) {
    // This needs to be fixed, if the updates are called in a specific order
    // Pieces can tend to "glide" all the way to the end of a conveyor belt row
    if (hasPiece()) {
        raylib::Vector2 tilePosition        = board.getTilePosition(this);
        raylib::Vector2 destinationPosition = board.getTilePosition(this);

        switch (direction) {
            case UP:
                destinationPosition = tilePosition + raylib::Vector2(0, 1);
                break;
            case DOWN:
                destinationPosition = tilePosition + raylib::Vector2(0, -1);
                break;
            case LEFT:
                destinationPosition = tilePosition + raylib::Vector2(-1, 0);
                break;
            case RIGHT:
                destinationPosition = tilePosition + raylib::Vector2(1, 0);
                break;
        }

        Tile* destinationTile = board.getTile(destinationPosition.x, destinationPosition.y);

        if (destinationTile) {
            // Queue a movement to the destination tile
            board.addQueuedMove({ 
                destinationTile, 
                this, 
                false, 
                createSlideAnimation(raylib::Vector3(0, 0, 0), raylib::Vector3(destinationPosition.x - tilePosition.x, destinationPosition.y - tilePosition.y, 0))
            });
        }
    }

    lifetime--;
}


PortalTile::PortalTile(raylib::Texture2D* texture, int portalNumber) : Tile(10), atlas(texture), portalNumber(portalNumber) { }

void PortalTile::draw(RenderQueue& renderQueue, int x, int y, float z, bool selected, bool hide) {
    TileType tileType = ((x + y) % 2 == 0) ? TILE_WHITE_CUBE : TILE_BLACK_CUBE;

    TilePosition tile = tileData[tileType];

    Rectangle source = { tile.tileX * TILE_SIZE, tile.tileY * TILE_SIZE, TILE_SIZE, TILE_SIZE };

    Color portalColor;

    switch (portalNumber % 3) {
        case 0:
			portalColor = ORANGE;
			break;
        case 1:
			portalColor = GREEN;
            break;
		case 2:
            portalColor = PURPLE;
            break;
    }

    renderQueue.addSpriteObject(SpriteObject(raylib::Vector3(x, y, z - 1), atlas, source, selected ? RED : portalColor));

    if (currentPiece != nullptr) {
        currentPiece->draw(renderQueue, x, y, z, hide);
    }
}

void PortalTile::updateState(Board& board) {
    if (hasPiece()) {
		// Get a list of all portal tiles
        vector<Tile*> portalTiles = board.getTilesOfType<PortalTile>();

        // Find the destination portal tile with the same portal number
        PortalTile* destinationPortal = nullptr;
        for (Tile* portalTile : portalTiles) {
			PortalTile* portal = dynamic_cast<PortalTile*>(portalTile);
            if (portal != this && portal->portalNumber == this->portalNumber) {
                destinationPortal = portal;
                break;
            }
        }

        if (destinationPortal) {
            // Get the positions of the current and destination portal tiles
            raylib::Vector2 currentPosition = board.getTilePosition(this);
            raylib::Vector2 destinationPosition = board.getTilePosition(destinationPortal);

            // Queue a movement to the destination portal tile
            board.addQueuedMove({
                destinationPortal,
                this,
                false,
                createTeleportAnimation(raylib::Vector3(0, 0, 0), raylib::Vector3(destinationPosition.x - currentPosition.x, destinationPosition.y - currentPosition.y, 0))
            });

            lifetime = 0;
			destinationPortal->setLifetime(0);
        }
    }
}