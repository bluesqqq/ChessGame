#include "board.h"
#include <cmath>
#include <algorithm>

void Board::drawTile(RenderQueue& renderQueue, int row, int col, TileType type) {
    TilePosition tile = tileData[type];
    Rectangle source = { tile.tileX * TILE_SIZE, tile.tileY * TILE_SIZE, TILE_SIZE, TILE_SIZE };
    renderQueue.addSpriteObject(SpriteObject(raylib::Vector3(row, col, -1), atlas, source));
}

Board::Board(raylib::Texture2D* texture, vector<Player>& players) : atlas(texture), players(players) {
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

void Board::draw(RenderQueue& renderQueue, int player, int x, int y) {
    bool hide = false; 

    std::vector<std::pair<int, int>> highlightTiles;

    if (isPlayable()) {
        // If piece is selected, hide the other pieces
        if (x >= 0 && x < 8 && y >= 0 && y < 8 && tiles[x][y]->hasPiece()) {
            if (tiles[x][y]->getPiece()->getPlayer() == player) {
                hide = true;
                highlightTiles = tiles[x][y]->getPiece()->getLegalMoves(x, y, *this);
                hide = true;
            }
        }
    }

    float time = GetTime(); // Get elapsed time

    for (int row = -1; row <= 8; row++) {
        for (int col = -1; col <= 8; col++) {
            if (row == -1 && col == -1) {
                drawTile(renderQueue, row, col, TILE_SE_CORNER);
                continue;
            }
            else if (row == -1 && col == 8) {
                drawTile(renderQueue, row, col, TILE_NE_CORNER);
                continue;
            }
            else if (row == 8 && col == -1) {
                drawTile(renderQueue, row, col, TILE_SW_CORNER);
                continue;
            }
            else if (row == 8 && col == 8) {
                drawTile(renderQueue, row, col, TILE_NW_CORNER);
                continue;
            }
            else if (row == -1) {
                drawTile(renderQueue, row, col, TILE_EAST_WALL);
                continue;
            }
            else if (row == 8) {
                drawTile(renderQueue, row, col, TILE_WEST_WALL);
                continue;
            }
            else if (col == -1) {
                drawTile(renderQueue, row, col, TILE_SOUTH_WALL);
                continue;
            }
            else if (col == 8) {
                drawTile(renderQueue, row, col, TILE_NORTH_WALL);
                continue;
            }

            std::pair<int, int> current_pair = { row, col };
            auto it = std::find(highlightTiles.begin(), highlightTiles.end(), current_pair);

            Tile* tile = tiles[row][col];

            // Apply sine wave for a wavy effect
            float waveOffset = std::max(sin(time + (row + col) * 0.4f) * 0.2f, 0.0f);

            if (x == row && y == col) // Mouse is hovered
            {
                tile->draw(renderQueue, row, col, waveOffset, true, false);
            }
            else if (it != highlightTiles.end()) // Possible moves on hovered piece
            {
                tile->draw(renderQueue, row, col, waveOffset, true, false);
            }
            else // Normal rendering
            {
                tile->draw(renderQueue, row, col, waveOffset, false, hide);
            }
        }
    }

}

void Board::update() {
    // Update all tiles
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            tiles[row][col]->update();
        }
    }

    double currentTime = GetTime();

    // Check if there are any moves
    if (!queuedMoves.empty()) {
        bool unfinishedAnimations = false;

        // Check if any moves have unfinished animations
		for (auto& move : queuedMoves) {
            if (!move.animation.ended()) { 
                unfinishedAnimations = true;
            }
		}

        if (unfinishedAnimations) {
            for (auto& move : queuedMoves) {
                move.animation.currentTime = currentTime;

                Piece* animatingPiece = move.from->getPiece();

				if (animatingPiece) {
					raylib::Vector3 animationOffset = move.animation.getPosition();

					animatingPiece->setOffset(animationOffset);
				}
            }
        } else {
			executeQueuedMoves();
        }
    } else if (updateStatePhase) {
        removeExpiredTiles();
		spawnRandomTiles();

        // Finish update state phase
		updateStatePhase = false;
    }
}

void Board::updateState() {
    updateStatePhase = true;
    // Update the state of every tile
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            tiles[row][col]->updateState(*this);
        }
    }

    // Remove any conflicting moves added to the queuedMoves
	removeConflictingMoves();

	// Start animations for all queued moves
    for (auto& move : queuedMoves) {
		move.animation.startAnimation();
    }
}

void Board::removeExpiredTiles() {
    // Set any expired tiles back to BasicTiles
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Tile* tile = tiles[row][col];
            if (tile->getLifetime() == 0) {
                changeTile(row, col, new BasicTile(atlas));
            }
        }
    }
}

bool Board::isPlayable() {
    return (queuedMoves.empty());
}

void Board::addQueuedMove(Move move) {
    queuedMoves.push_back(move);
}

void Board::removeConflictingMoves() {
    // Remove conflicting moves (moves with the same destination)
    for (auto it = queuedMoves.begin(); it != queuedMoves.end(); ++it) {
        for (auto jt = std::next(it); jt != queuedMoves.end();) {
            if (jt->to == it->to) {
                jt = queuedMoves.erase(jt); // Remove conflicting move
                continue;
            }
            ++jt;
        }
    }

    // Remove non-overtaking moves that have a stationary piece in their destination
    bool removedMove;
    do {
        removedMove = false;
        for (auto it = queuedMoves.begin(); it != queuedMoves.end();) {
            if (!it->canOvertake && it->to->hasPiece()) {
                auto blockingMove = std::find_if(queuedMoves.begin(), queuedMoves.end(), [it](const Move& other) {
                    return (other.from == it->to) && (other.to != it->to);
                    });

                if (blockingMove == queuedMoves.end()) {
                    it = queuedMoves.erase(it);
                    removedMove = true;
                    continue;
                }
            }
            ++it;
        }
    } while (removedMove);
}

void Board::executeQueuedMoves() {
    for (auto& move : queuedMoves) {
        Piece* animatingPiece = move.from->getPiece();

        if (animatingPiece) {
            animatingPiece->setOffset({ 0.0, 0.0, 0.0 });
        } else {
            Vector2 location = getTilePosition(move.from);
            cout << "ERROR: cannot find piece at location: " << location.x << ", " << location.y << endl;
        }

        move.to->queuePiece(move.from->removePiece());
    }

    // Dequeue all the pieces and complete the move
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            tiles[row][col]->dequeuePiece();
        }
    }

    queuedMoves.clear();
}

Tile* Board::setTile(int row, int col, Tile* newTile) {
    Tile* oldTile = tiles[row][col];
    tiles[row][col] = newTile;

    return oldTile;
}

Tile* Board::changeTile(int row, int col, Tile* newTile) {
    Tile* oldTile = setTile(row, col, newTile);

    // Set the new tile's piece to the old tile's piece
    if (oldTile) {
        newTile->setPiece(oldTile->removePiece());
    }

    return oldTile;
}

Tile* Board::getTile(int row, int col) {
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

template <typename T>
vector<Tile*> Board::getTilesOfType() {
    vector<Tile*> tiles;

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Tile* tile = getTile(row, col);

            if (tile) {
                if (dynamic_cast<T*>(tile) != nullptr) {
                    tiles.push_back(tile);
                }
            }
        }
    }

    return tiles;
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

bool Board::canMove(int player) {
    vector<pair<int, int>> playerPieceLocations = getPlayersPieces(player);

    for (pair<int, int> location : playerPieceLocations) {
        Tile* tile = getTile(location.first, location.second);

        Piece* piece = tile->getPiece();

        if (!piece->getLegalMoves(location.first, location.second, *this).empty()) {
            return true; // Found a legal move
        }
    }

    return false;
}

bool Board::isInCheckmate(int player) {
    if (!isInCheck(player)) return false; // Cannot be in checkmate if not in check

    return !canMove(player);
}

bool Board::isInStalemate(int player) {
    if (isInCheck(player)) return false; // Cannot be in stalemate if in check

    return !canMove(player);
}

void Board::spawnRandomTiles() {
    int randSpawn = rand() % 10;

    switch (randSpawn) {
        case 0:
            spawnRandomTiles(TileSpawnType::ICE_SPAWN);
            break;
        case 1:
            spawnRandomTiles(TileSpawnType::CONVEYOR_LOOP_SPAWN);
            break;
        case 2:
            spawnRandomTiles(TileSpawnType::CONVEYOR_ROW_SPAWN);
            break;
        case 3:
            spawnRandomTiles(TileSpawnType::PORTAL_SPAWN);
            break;
    }
}

void Board::spawnRandomTiles(TileSpawnType type) {
    switch (type) {
        case TileSpawnType::PORTAL_SPAWN: {
            Tile* spawnTileFirst;
            Tile* spawnTileSecond;

            raylib::Vector2 spawnPositionFirst(rand() % 8, rand() % 8);
            raylib::Vector2 spawnPositionSecond(rand() % 8, rand() % 8);

            spawnTileFirst = getTile(spawnPositionFirst.x, spawnPositionFirst.y);
            spawnTileSecond = getTile(spawnPositionSecond.x, spawnPositionSecond.y);

			// Keep trying to spawn until two empty tiles are found (that are not the same)
            while (spawnTileFirst->hasPiece() || spawnTileSecond->hasPiece() || spawnPositionFirst == spawnPositionSecond) {
                spawnPositionFirst = raylib::Vector2(rand() % 8, rand() % 8);
                spawnPositionSecond = raylib::Vector2(rand() % 8, rand() % 8);

                spawnTileFirst = getTile(spawnPositionFirst.x, spawnPositionFirst.y);
                spawnTileSecond = getTile(spawnPositionSecond.x, spawnPositionSecond.y);
            }

            // Create two linked portals
			changeTile(spawnPositionFirst.x,  spawnPositionFirst.y,  new PortalTile(atlas, portalCounter));
            changeTile(spawnPositionSecond.x, spawnPositionSecond.y, new PortalTile(atlas, portalCounter));

            portalCounter++;
            break;
        }

        case TileSpawnType::ICE_SPAWN: {
            while (true) {
                raylib::Vector2 spawnPosition(rand() % 8, rand() % 8);

                Tile* spawnTile = getTile(spawnPosition.x, spawnPosition.y);
                if (dynamic_cast<BasicTile*>(spawnTile) != nullptr) {
                    changeTile(spawnPosition.x, spawnPosition.y, new IceTile(atlas));
                    return;
                }
            }
            break;
        }

        case TileSpawnType::BREAK_SPAWN: {
            while (true) {
                raylib::Vector2 spawnPosition(rand() % 8, rand() % 8);

                Tile* spawnTile = getTile(spawnPosition.x, spawnPosition.y);
                if (dynamic_cast<BasicTile*>(spawnTile) != nullptr) {
                    changeTile(spawnPosition.x, spawnPosition.y, new BreakingTile(atlas));
                    return;
                }
            }
            break;
        }

        case TileSpawnType::CONVEYOR_ROW_SPAWN: {
            int row = rand() % 4 + 2; // can only spawn on rows 3 - 6
            for (int i = 0; i < 8; i++) {
                changeTile(i, row, new ConveyorTile(atlas, RIGHT));
            }
            break;
        }

        case TileSpawnType::CONVEYOR_LOOP_SPAWN: {
            int width = rand() % 3 + 2;  // width of 2 - 4
            int length = rand() % 3 + 2; // length of 2 - 4

            int x = rand() % (8 - width);
            int y = rand() % (8 - width);

            bool clockwise = false; // rand() % 2; // Random direction

            int cw = clockwise ? 0 : 1;

            // Top row
            for (int i = cw; i < width - 1 + cw; i++) {
                changeTile(x + i, y, new ConveyorTile(atlas, clockwise ? RIGHT : LEFT));
            }

            for (int i = cw; i < length - 1 + cw; i++) {
                changeTile(x + width - 1, y + i, new ConveyorTile(atlas, clockwise ? UP : DOWN));
            }

            // Bottom row
            for (int i = cw; i < width - 1 + cw; i++) {
                changeTile(x + (width - 1) - i, y + (length - 1), new ConveyorTile(atlas, clockwise ? LEFT : RIGHT));
            }

            // Left column
            for (int i = cw; i < length - 1 + cw; i++) {
                changeTile(x, y + (length - 1) - i, new ConveyorTile(atlas, clockwise ? DOWN : UP));
            }

            break;
        }
    }
}

template <typename T>
int Board::getTileCount() {
    int count = 0;

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            if (dynamic_cast<T*>(tiles[row][col])) {
                ++count;
            }
        }
    }

    return count;
}

template int Board::getTileCount<ConveyorTile>();
template int Board::getTileCount<IceTile>();
template int Board::getTileCount<BreakingTile>();
template int Board::getTileCount<PortalTile>();

template std::vector<Tile*> Board::getTilesOfType<PortalTile>();
