#include "board.h"
#include <cmath>
#include <algorithm>
#include "Theme.h"

void Board::drawTile(RenderQueue& renderQueue, int rank, int file, TileType type) {
    TilePosition tile = tileData[type];
    Rectangle source = { tile.tileX * TILE_SIZE, tile.tileY * TILE_SIZE, TILE_SIZE, TILE_SIZE };
    renderQueue.addSpriteObject(SpriteObject(raylib::Vector3(rank, file, -1), atlas, source));
}

Board::Board(raylib::Texture2D* texture, vector<Player>& players) : atlas(texture), players(players) {
    // Populate with generic tiles
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            tiles[rank][file] = new BasicTile(atlas);
        }
    }

	// Place Pawns in the second and seventh ranks
    for (int file = 0; file < 8; file++) {
        tiles[1][file]->setPiece(new Pawn(atlas, 1)); // Player 1 Pawns
        tiles[6][file]->setPiece(new Pawn(atlas, 2)); // Player 2 Pawns
    }

    // Place Rooks
    tiles[0][0]->setPiece(new Rook(atlas, 1));
    tiles[0][7]->setPiece(new Rook(atlas, 1));
    tiles[7][0]->setPiece(new Rook(atlas, 2));
    tiles[7][7]->setPiece(new Rook(atlas, 2));

    // Place Knights
    tiles[0][1]->setPiece(new Knight(atlas, 1));
    tiles[0][6]->setPiece(new Knight(atlas, 1));
    tiles[7][1]->setPiece(new Knight(atlas, 2));
    tiles[7][6]->setPiece(new Knight(atlas, 2));

    // Place Bishops
    tiles[0][2]->setPiece(new Bishop(atlas, 1));
    tiles[0][5]->setPiece(new Bishop(atlas, 1));
    tiles[7][2]->setPiece(new Bishop(atlas, 2));
    tiles[7][5]->setPiece(new Bishop(atlas, 2));

    // Place Queens
    tiles[0][3]->setPiece(new Queen(atlas, 1)); 
    tiles[7][3]->setPiece(new Queen(atlas, 2)); 

    // Place Kings
    tiles[0][4]->setPiece(new King(atlas, 1));
    tiles[7][4]->setPiece(new King(atlas, 2));
}

void Board::draw(Theme& theme, RenderQueue& renderQueue, int player, Cell selectedCell) {
    bool hide = false; 

    std::vector<Cell> highlightTiles; // List of tiles to be highlighted

    if (isPlayable()) { // Highlight selected tiles if in play
        // If piece is selected, hide the other pieces
        if (selectedCell.rank > 0 && selectedCell.rank < 9 && selectedCell.file > 0 && selectedCell.file < 9 && getTile(selectedCell)->hasPiece()) {
            if (getTile(selectedCell)->getPiece()->getPlayer() == player) {
                hide = true;
                highlightTiles = getTile(selectedCell)->getPiece()->getLegalMoves(*this);
                hide = true;
            }
        }
    }

    float time = GetTime(); // Get elapsed time

    for (int rank = -1; rank <= 8; rank++) {
        for (int file = -1; file <= 8; file++) {

			raylib::Vector2 tilePosition = cellToScreenPosition(Cell(rank + 1, file + 1));

            if (rank == -1 && file == -1) {
                drawTile(renderQueue, rank, file, TILE_SE_CORNER);
                continue;
            }
            else if (rank == -1 && file == 8) {
                drawTile(renderQueue, rank, file, TILE_NE_CORNER);
                continue;
            }
            else if (rank == 8 && file == -1) {
                drawTile(renderQueue, rank, file, TILE_SW_CORNER);
                continue;
            }
            else if (rank == 8 && file == 8) {
                drawTile(renderQueue, rank, file, TILE_NW_CORNER);
                continue;
            }
            else if (rank == -1) {
                drawTile(renderQueue, rank, file, TILE_EAST_WALL);
                continue;
            }
            else if (rank == 8) {
                drawTile(renderQueue, rank, file, TILE_WEST_WALL);
                continue;
            }
            else if (file == -1) {
                drawTile(renderQueue, rank, file, TILE_SOUTH_WALL);
                continue;
            }
            else if (file == 8) {
                drawTile(renderQueue, rank, file, TILE_NORTH_WALL);
                continue;
            }

            Cell currentCell = Cell(rank + 1, file + 1); // make this whole function more representative of this

            auto it = std::find(highlightTiles.begin(), highlightTiles.end(), currentCell);

            Tile* tile = tiles[rank][file];

            // Apply sine wave for a wavy effect
            float waveOffset = std::max(sin(time + (rank + file) * 0.4f) * 0.2f, 0.0f);

            if (selectedCell.rank - 1 == rank && selectedCell.file - 1 == file) { // Mouse is hovered
                tile->draw(theme, renderQueue, tilePosition.x, tilePosition.y, waveOffset, true, false);
            } else if (it != highlightTiles.end()) { // Possible moves on hovered piece
                tile->draw(theme, renderQueue, tilePosition.x, tilePosition.y, waveOffset, true, false);
            } else { // Normal rendering
                tile->draw(theme, renderQueue, tilePosition.x, tilePosition.y, waveOffset, false, hide);
            }
        }
    }
}

raylib::Vector2 Board::cellToScreenPosition(Cell cell) {
	raylib::Vector2 position = { (float)(cell.file - 1), (float)(8 - cell.rank) }; // Draws a1 to the left, h8 to the right
	return position;
}

raylib::Vector3 Board::cellToIsoPosition(Cell cell)
{
    raylib::Vector3 position = { (float)(cell.file - 1), (float)(8 - cell.rank), 0 }; // Draws a1 to the left, h8 to the right
    return position;
}

void Board::update() {
    // Update all tiles
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            tiles[rank][file]->update();
        }
    }

    double currentTime = GetTime();

    if (!queuedMoves.empty()) { // Continued unfinished moves until all are gone
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

                Piece* animatingPiece = getTile(move.from)->getPiece();

				if (animatingPiece) {
					raylib::Vector3 animationOffset = move.animation.getPosition();

					animatingPiece->setOffset(animationOffset);
				}
            }
        } else {
			executeQueuedMoves();

            promotePieces();
        }
    } else if (hasPromotion()) { // Continued unfinished promotions until all are gone

    } else if (updateStatePhase) { // Finish up
        removeExpiredTiles();
		spawnRandomTiles();

        // Finish update state phase
		updateStatePhase = false;
    }
}

void Board::updateState() {
    updateStatePhase = true;
    // Update the state of every tile
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            tiles[rank][file]->updateState(*this);
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
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            Tile* tile = tiles[rank][file];
            if (tile->getLifetime() == 0) {
                changeTile(rank, file, new BasicTile(atlas));
            }
        }
    }
}

bool Board::isPlayable() {
    return (queuedMoves.empty() && promotions.empty());
}

void Board::queueMove(Move move) {
    move.animation.startAnimation(); // Start the move's animation

    // It's debatable whether or not tiles that move the pieces should count as a piece move, but I'm going to say yes
	getTile(move.from)->getPiece()->move();

    queuedMoves.push_back(move); // Add the move to the queue
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
			Cell to = it->to;
            if (!it->canOvertake && getTile(to)->hasPiece()) {
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
        Piece* animatingPiece = getTile(move.from)->getPiece();

        if (animatingPiece) {
            animatingPiece->setOffset({ 0.0, 0.0, 0.0 });
        } else {
            cout << "ERROR: cannot find piece at location: " << move.from.rank << ", " << move.from.file << endl;
        }

        getTile(move.to)->queuePiece(getTile(move.from)->removePiece());
    }

    // Dequeue all the pieces and complete the move
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            tiles[rank][file]->dequeuePiece();
        }
    }

    queuedMoves.clear();
}

Tile* Board::setTile(int rank, int file, Tile* newTile) {
    Tile* oldTile = tiles[rank][file];
    tiles[rank][file] = newTile;

    return oldTile;
}

Tile* Board::changeTile(int rank, int file, Tile* newTile) {
    Tile* oldTile = setTile(rank, file, newTile);

    // Set the new tile's piece to the old tile's piece
    if (oldTile) {
        newTile->setPiece(oldTile->removePiece());
    }

    return oldTile;
}

Tile* Board::getTile(int rank, int file) {
    // If out of bounds, return a nullptr
    if (rank >= 0 && rank < 8 && file >= 0 && file < 8)
        return tiles[rank][file];

    return nullptr;
}

raylib::Vector2 Board::getTilePosition(Tile* tile) {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            if (tiles[rank][file] == tile) {
                return raylib::Vector2(rank, file);
            }
        }
    }

    return { -1.0f, -1.0f }; // Return an invalid position if the tile isn't found
}

Piece* Board::movePiece(int player, Cell piece, Cell move) {
    Tile* startTile = getTile(piece);
    Tile* endTile = getTile(move);

    Piece* targetPiece = startTile->removePiece();

    if (!targetPiece) return nullptr;

    Piece* discardedPiece = nullptr;

    // if the destination tile has a piece, remove it and store it as the discarded piece
    if (endTile->hasPiece()) {
        discardedPiece = endTile->removePiece();
    }

    targetPiece->move();

    endTile->setPiece(targetPiece);

    return discardedPiece;
}


bool Board::isLegalMove(int player, Cell piece, Cell move) {
    // Get the start and end tiles for this move
    Tile* startTile = getTile(piece);
    Tile* endTile = getTile(move);

    // Check if both tiles exist
    if (!startTile || !endTile) return false;

    // Check if the start tile has a piece
    if (!startTile->hasPiece()) return false;

    Piece* movePiece = startTile->getPiece();

    // Check if the piece is owned by the player making this move
    if (movePiece->getPlayer() != player) return false;

    // Check if this move is in the list of legal moves for the selected piece
    if (!movePiece->isLegalMove(*this, move)) return false;

    return true;
}


vector<Move> Board::getAllLegalMoves(int player) {
    vector<Move> allLegalMoves;

    std::vector<Cell> pieceLocations = getPlayersPieces(player);

    for (Cell& pieceLocation : pieceLocations) {
        Tile* tile = getTile(pieceLocation);

        Piece* piece = tile->getPiece();

        std::vector<Cell> pieceMoves = piece->getLegalMoves(*this);

        for (Cell& move : pieceMoves) {
            Tile* from = tile;
            Tile* to = getTile(move);

            Move m = {
                move,
                pieceLocation,
                true,
                createInstantAnimation()
            };
            allLegalMoves.push_back(m);
        }
    }

    return allLegalMoves;
}

std::vector<Cell> Board::getPlayersPieces(int player) {
    std::vector<Cell> locations;

    for (int rank = 1; rank <= 8; rank++) {
        for (int file = 1; file <= 8; file++) {
            Tile* tile = getTile(Cell(rank,file));

            if (tile) {
                if (tile->hasPiece()) {
                    Piece* piece = tile->getPiece();

                    if (piece->getPlayer() == player) {
                        locations.push_back(Cell(rank, file));
                    }
                }
            }
        }
    }

    return locations;
}

template <typename T>
std::vector<Cell> Board::getPlayersPiecesOfType(int player) {
    std::vector<Cell> locations;

    for (int rank = 1; rank <= 8; rank++) {
        for (int file = 1; file <= 8; file++) {
            Tile* tile = getTile(Cell(rank, file));

            if (tile) {
                if (tile->hasPiece()) {
                    Piece* piece = tile->getPiece();

                    if (piece->getPlayer() == player) {
                        // Check if the piece is the type that the function is looking for
                        if (dynamic_cast<T*>(piece) != nullptr) {
                            locations.push_back(Cell(rank, file));
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

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            Tile* tile = getTile(rank, file);

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
    std::vector<Cell> kings = getPlayersPiecesOfType<King>(player);

    if (kings.empty()) return false; // Should not happen in a normal game

    Cell kingsPosition = kings[0];

    // Get all the possible moves of the opposing player's pieces
    std::vector<Cell> opponentPiecesLocations = getPlayersPieces((player % 2) + 1);

    for (Cell& location : opponentPiecesLocations) {
        Tile* tile = getTile(location);

        Piece* piece = tile->getPiece();

        std::vector<Cell> validMoves = piece->getValidMoves(*this);

        for (Cell& validMove : validMoves) {
            if (validMove == kingsPosition) return true;
        }
    }

    return false;
}

bool Board::canMove(int player) {
    std::vector<Cell> playerPieceLocations = getPlayersPieces(player);

    for (Cell& location : playerPieceLocations) {
        Tile* tile = getTile(location);

        Piece* piece = tile->getPiece();

        if (!piece->getLegalMoves(*this).empty()) {
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
    int randSpawn = rand() % 20;

    switch (randSpawn) {
        case 0:
            spawnRandomTiles(TileSpawnType::CONVEYOR_LOOP_SPAWN);
            break;
        case 1:
            spawnRandomTiles(TileSpawnType::CONVEYOR_ROW_SPAWN);
            break;

        case 2:
            spawnRandomTiles(TileSpawnType::ICE_SPAWN);
            break;
        case 3:
            spawnRandomTiles(TileSpawnType::ICE_SPAWN);
            break;
        case 4:
            spawnRandomTiles(TileSpawnType::PORTAL_SPAWN);
            break;
        case 5:
            spawnRandomTiles(TileSpawnType::PORTAL_SPAWN);
            break;
        case 6:
            spawnRandomTiles(TileSpawnType::BREAK_SPAWN);
            break;
        case 7:
            spawnRandomTiles(TileSpawnType::BREAK_SPAWN);
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
            int rank = rand() % 4 + 2; // can only spawn on ranks 3 - 6
            for (int i = 0; i < 8; i++) {
                changeTile(i, rank, new ConveyorTile(atlas, RIGHT));
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

            // Top rank
            for (int i = cw; i < width - 1 + cw; i++) {
                changeTile(x + i, y, new ConveyorTile(atlas, clockwise ? RIGHT : LEFT));
            }

            for (int i = cw; i < length - 1 + cw; i++) {
                changeTile(x + width - 1, y + i, new ConveyorTile(atlas, clockwise ? UP : DOWN));
            }

            // Bottom rank
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

    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            if (dynamic_cast<T*>(tiles[rank][file])) {
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


template std::vector<Cell> Board::getPlayersPiecesOfType<Rook>(int player);