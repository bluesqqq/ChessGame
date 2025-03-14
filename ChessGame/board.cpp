#include "board.h"
#include <cmath>
#include <algorithm>

void Board::drawTile(int row, int col, TileType type) {
    TilePosition tile = tileData[type];
    Rectangle source = { tile.tileX * TILE_SIZE, tile.tileY * TILE_SIZE, TILE_SIZE, TILE_SIZE };
    Vector2 position = IsoToScreen(row, col, 0);
    DrawTextureRec(*atlas, source, position, WHITE);
}

Board::Board(Texture2D* texture, vector<Player>& players) : atlas(texture), players(players) {
    // Populate with generic tiles
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            tiles[row][col] = new BasicTile(atlas);
        }
    }

    // Place Pawns
    for (int row = 0; row < 8; row++) {
        tiles[row][1]->setPiece(new Pawn(atlas, 1)); // Player 1 Pawns
        tiles[row][6]->setPiece(new Pawn(atlas, 2)); // Player 2 Pawns
    }

    // Place Rooks
    tiles[0][0]->setPiece(new Rook(atlas, 1));
    tiles[7][0]->setPiece(new Rook(atlas, 1));
    tiles[0][7]->setPiece(new Rook(atlas, 2));
    tiles[7][7]->setPiece(new Rook(atlas, 2));

    // Place Knights
    tiles[1][0]->setPiece(new Knight(atlas, 1));
    tiles[6][0]->setPiece(new Knight(atlas, 1));
    tiles[1][7]->setPiece(new Knight(atlas, 2));
    tiles[6][7]->setPiece(new Knight(atlas, 2));

    // Place Bishops
    tiles[2][0]->setPiece(new Bishop(atlas, 1));
    tiles[5][0]->setPiece(new Bishop(atlas, 1));
    tiles[2][7]->setPiece(new Bishop(atlas, 2));
    tiles[5][7]->setPiece(new Bishop(atlas, 2));

    // Place Queens
    tiles[4][0]->setPiece(new Queen(atlas, 1));  // Black Queen
    tiles[4][7]->setPiece(new Queen(atlas, 2));   // White Queen

    // Place Kings
    tiles[3][0]->setPiece(new King(atlas, 1));  // Black King
    tiles[3][7]->setPiece(new King(atlas, 2));   // White King

}

void Board::draw(int player, int x, int y) {
    bool hide = false; 

    std::vector<std::pair<int, int>> highlightTiles;

    // If piece is selected, hide the other pieces
    if (x >= 0 && x < 8 && y >= 0 && y < 8 && tiles[x][y]->hasPiece()) {
        if (tiles[x][y]->getPiece()->getPlayer() == player) {
            hide = true;
            highlightTiles = tiles[x][y]->getPiece()->getLegalMoves(x, y, *this);
            hide = true;
        }
    }

    float time = GetTime(); // Get elapsed time

    /*
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            std::pair<int, int> current_pair = { row, col };
            auto it = std::find(highlightTiles.begin(), highlightTiles.end(), current_pair);

            Tile* tile = tiles[row][col];

            // Apply sine wave for a wavy effect
            float waveOffset = std::max(sin(time + (row + col) * 0.4f) * 0.2f, 0.0f);

            if (x == row && y == col) // Mouse is hovered
            {
                tile->draw(row, col, waveOffset, true, false);
            }
            else if (it != highlightTiles.end()) // Possible moves on hovered piece
            {
                tile->draw(row, col, waveOffset, true, false);
            }
            else // Normal rendering
            {
                tile->draw(row, col, waveOffset, false, hide);
            }
        }
    }
    */

    for (int row = -1; row <= 8; row++) {
        for (int col = -1; col <= 8; col++) {
            if (row == -1 && col == -1) {
                drawTile(row, col, TILE_SE_CORNER);
                continue;
            }
            else if (row == -1 && col == 8) {
                drawTile(row, col, TILE_NE_CORNER);
                continue;
            }
            else if (row == 8 && col == -1) {
                drawTile(row, col, TILE_SW_CORNER);
                continue;
            }
            else if (row == 8 && col == 8) {
                drawTile(row, col, TILE_NW_CORNER);
                continue;
            }
            else if (row == -1) {
                drawTile(row, col, TILE_EAST_WALL);
                continue;
            }
            else if (row == 8) {
                drawTile(row, col, TILE_WEST_WALL);
                continue;
            }
            else if (col == -1) {
                drawTile(row, col, TILE_SOUTH_WALL);
                continue;
            }
            else if (col == 8) {
                drawTile(row, col, TILE_NORTH_WALL);
                continue;
            }

            std::pair<int, int> current_pair = { row, col };
            auto it = std::find(highlightTiles.begin(), highlightTiles.end(), current_pair);

            Tile* tile = tiles[row][col];

            // Apply sine wave for a wavy effect
            float waveOffset = std::max(sin(time + (row + col) * 0.4f) * 0.2f, 0.0f);

            if (x == row && y == col) // Mouse is hovered
            {
                tile->draw(row, col, waveOffset, true, false);
            }
            else if (it != highlightTiles.end()) // Possible moves on hovered piece
            {
                tile->draw(row, col, waveOffset, true, false);
            }
            else // Normal rendering
            {
                tile->draw(row, col, waveOffset, false, hide);
            }
        }
    }

}

void Board::update() {

    // Set any expired tiles back to BasicTiles
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Tile* tile = tiles[row][col];
            if (tile->getLifetime() == 0) {
                changeTile(row, col, new BasicTile(atlas));
            }
        }
    }

    // Update every tile
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            tiles[row][col]->update(*this);
        }
    }

    // Rudimentary random spawning tiles system
    int randSpawn = rand() % 10;

    switch (randSpawn) {
        case 0:
            spawnRandomTiles(TileSpawnType::CONVEYOR_ROW_SPAWN);
            break;
        case 1:
            spawnRandomTiles(TileSpawnType::ICE_SPAWN);
            break;
    }
}

Tile* Board::setTile(int row, int col, Tile* newTile)
{
    Tile* oldTile = tiles[row][col];
    tiles[row][col] = newTile;

    return oldTile;
}

Tile* Board::changeTile(int row, int col, Tile* newTile) {
    Tile* oldTile = tiles[row][col];
    tiles[row][col] = newTile;

    if (oldTile) {
        newTile->setPiece(oldTile->removePiece());
    }

    return oldTile;
}

Tile* Board::getTile(int row, int col)
{
    // If out of bounds, return a nullptr
    if (row >= 0 && row < 8 && col >= 0 && col < 8)
        return tiles[row][col];

    return nullptr;
}

raylib::Vector2 Board::getTilePosition(Tile* tile) {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (tiles[row][col] == tile) {
                return raylib::Vector2(row, col);
            }
        }
    }

    return { -1.0f, -1.0f }; // Return an invalid position if the tile isn't found
}

Piece* Board::movePiece(int player, int pieceRow, int pieceCol, int destinationRow, int destinationCol)
{
    Tile* startTile = getTile(pieceRow, pieceCol);
    Tile* endTile = getTile(destinationRow, destinationCol);

    Piece* targetPiece = startTile->removePiece();

    if (!targetPiece) return nullptr;

    Piece* discardedPiece = nullptr;

    // if the destination tile has a piece, remove it and store it as the discarded piece
    if (endTile->hasPiece())
    {
        Piece* discardedPiece = endTile->removePiece();
    }

    targetPiece->move();

    endTile->setPiece(targetPiece);

    return discardedPiece;
}


bool Board::isLegalMove(int player, int pieceRow, int pieceCol, int destinationRow, int destinationCol) {
    // Get the start and end tiles for this move
    Tile* startTile = getTile(pieceRow, pieceCol);
    Tile* endTile = getTile(destinationRow, destinationCol);

    // Check if both tiles exist
    if (!startTile || !endTile) return false;

    // Check if the start tile has a piece
    if (!startTile->hasPiece()) return false;

    Piece* movePiece = startTile->getPiece();

    // Check if the piece is owned by the player making this move
    if (movePiece->getPlayer() != player) return false;

    // Check if this move is in the list of legal moves for the selected piece
    if (!movePiece->isLegalMove(pieceRow, pieceCol, *this, destinationRow, destinationCol)) return false;

    return true;
}

vector<pair<int, int>> Board::getPlayersPieces(int player)
{
    vector<pair<int, int>> locations;

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Tile* tile = getTile(row, col);

            if (tile) {
                if (tile->hasPiece()) {
                    Piece* piece = tile->getPiece();

                    if (piece->getPlayer() == player) {
                        locations.push_back({ row, col });
                    }
                }
            }
        }
    }

    return locations;
}

template <typename T>
vector<pair<int, int>> Board::getPlayersPiecesOfType(int player)
{
    vector<pair<int, int>> locations;

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Tile* tile = getTile(row, col);

            if (tile) {
                if (tile->hasPiece()) {
                    Piece* piece = tile->getPiece();

                    if (piece->getPlayer() == player) {
                        // Check if the piece is the type that the function is looking for
                        if (dynamic_cast<T*>(piece) != nullptr) {
                            locations.push_back({row, col});
                        }
                    }
                }
            }
        }
    }

    return locations;
}

bool Board::isInCheck(int player) {
    // Find the position of the player's king
    vector<pair<int, int>> kings = getPlayersPiecesOfType<King>(player);

    if (kings.empty()) return false; // Should not happen in a normal game

    pair<int, int> kingsPosition = kings[0];

    // Get all the possible moves of the opposing player's pieces
    vector<pair<int, int>> opponentPiecesLocations = getPlayersPieces((player % 2) + 1);

    for (pair<int, int> location : opponentPiecesLocations) {
        Tile* tile = getTile(location.first, location.second);

        Piece* piece = tile->getPiece();

        vector<pair<int, int>> validMoves = piece->getValidMoves(location.first, location.second, *this);

        for (pair<int, int> validMove : validMoves) {
            if (validMove == kingsPosition) return true;
        }
    }

    return false;
}

bool Board::noPossibleMoves(int player) {
    vector<pair<int, int>> playerPieceLocations = getPlayersPieces(player);

    for (pair<int, int> location : playerPieceLocations) {
        Tile* tile = getTile(location.first, location.second);

        Piece* piece = tile->getPiece();

        if (!piece->getLegalMoves(location.first, location.second, *this).empty()) {
            return false; // Found a legal move, not a checkmate
        }
    }

    return true;
}

bool Board::isInCheckmate(int player) {
    if (!isInCheck(player)) return false; // Cannot be in checkmate if not in check

    return noPossibleMoves(player);
}

bool Board::isInStalemate(int player) {
    if (isInCheck(player)) return false; // Cannot be in stalemate if in check

    return noPossibleMoves(player);
}

void Board::spawnRandomTiles(TileSpawnType type) {
    switch (type) {

        case TileSpawnType::ICE_SPAWN: {
            raylib::Vector2 spawnPosition(rand() % 8, rand() % 8);
            changeTile(spawnPosition.x, spawnPosition.y, new IceTile(atlas));
            break;
        }

        case TileSpawnType::CONVEYOR_ROW_SPAWN: {
            int row = rand() % 4 + 2; // can only spawn on rows 3 - 6
            for (int i = 0; i < 8; i++) {
                changeTile(i, row, new ConveyorTile(atlas, LEFT));
            }
            break;
        }
    }
}