#include "board.h"
#include <cmath>
#include <algorithm>
#include "Theme.h"
#include "animation.h"

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

    vector<Cell> highlightTiles; // List of tiles to be highlighted

    if (isPlayable()) { // Highlight selected tiles if in play
        // If piece is selected, hide the other pieces
        if (selectedCell.rank >= 0 && selectedCell.rank <= 7 && selectedCell.file >= 0 && selectedCell.file <= 7 && getTile(selectedCell)->hasPiece()) {
            if (getTile(selectedCell)->getPiece()->getPlayer() == player) {
                hide = true;

				vector<Move> legalMoves = getTile(selectedCell)->getPiece()->getLegalMoves(*this);

				for (Move& move : legalMoves) {

					highlightTiles.push_back(move.to);
				}

                hide = true;
            }
        }
    }

    float time = GetTime(); // Get elapsed time

    for (int rank = -1; rank <= 8; rank++) {
        for (int file = -1; file <= 8; file++) {

			raylib::Vector3 tilePosition = getIsoPositionAtCell(Cell(rank, file));

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

            Cell currentCell = Cell(rank, file); // make this whole function more representative of this

            auto it = find(highlightTiles.begin(), highlightTiles.end(), currentCell);

            Tile* tile = getTile(currentCell);

            // Apply sine wave for a wavy effect
            float waveOffset = max(sin(time + (rank + file) * 0.4f) * 0.2f, 0.0f);

            if (selectedCell.rank == rank && selectedCell.file == file) { // Mouse is hovered
                tile->draw(theme, renderQueue, tilePosition.x, tilePosition.y, waveOffset, true, false);
            } else if (it != highlightTiles.end()) { // Possible moves on hovered piece
                tile->draw(theme, renderQueue, tilePosition.x, tilePosition.y, waveOffset, true, false);
            } else { // Normal rendering
                tile->draw(theme, renderQueue, tilePosition.x, tilePosition.y, waveOffset, false, hide);
            }
        }
    }
}

Cell Board::getCell(Piece* piece) {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            if (tiles[rank][file]->hasPiece() && tiles[rank][file]->getPiece() == piece) {
                return Cell(rank, file);
            }
        }
    }

    return Cell(-1, -1);
}

Piece* Board::getPiece(Cell cell) {
    if (!cell.isInBounds()) return nullptr;

    Tile* tile = getTile(cell);

    if (tile) {
        return tile->getPiece();
    }

    return nullptr;
}

raylib::Vector3 Board::getIsoPositionAtCell(Cell cell) {
	raylib::Vector3 position = { (float)(cell.file), (float)(7 - cell.rank), 0 }; // Draws a1 to the left, h8 to the right
	return position;
}

raylib::Vector2 Board::getScreenPositionAtCell(Cell cell) {
    return IsoToScreen(getIsoPositionAtCell(cell));
}

Cell Board::getCellAtIsoPosition(raylib::Vector3 isoPosition) {
    return Cell(7 - isoPosition.y, isoPosition.x); // Ignore Z axis 
}

Cell Board::getCellAtScreenPosition(raylib::Vector2 screenPosition, raylib::Camera2D camera) {
    raylib::Vector2 translatedPosition = screenPosition - camera.offset;

    // Translate the screen position to isometric coordinates
    Vector3 isoPosition = ScreenToISO(translatedPosition, 0.0f); // NOTE: if moving the board anywhere on the z axis in the future, this will have to change

    return getCellAtIsoPosition(isoPosition);
}

void Board::update(int player) {
    // Update all tiles
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            tiles[rank][file]->update();
        }
    }

    double currentTime = GetTime();

    if (handlingPlayerTurn) {
        if (allMoveAnimationsFinished()) {
            executeQueuedMoves();

            applyAllTileEffects(); // Apply tile effects

            removeConflictingMoves(); // Remove any conflicting tile moves

            handlingPlayerTurn = false;
            handlingTileEffects = true;
        }
    } else if (handlingTileEffects) {
        if (allMoveAnimationsFinished()) {
            executeQueuedMoves();

            promotePieces(player);

            handlingPiecePromotion = true;
            handlingTileEffects = false;
        }
    } else if (handlingPiecePromotion) { // Continued unfinished promotions until all are gone
        if (!hasPromotion()) { // No more promotions left
            handlingStateUpdate = true;

            handlingPiecePromotion = false;
        }
    }
}

void Board::updateState() {
    cout << "Updated board state..." << endl;
    // Update the state of every tile
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            tiles[rank][file]->updateState(*this);
        }
    }

    removeExpiredTiles(); // Remove any tiles that expired

    spawnRandomTiles(); // Spawn random tiles;

    handlingStateUpdate = false;
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

bool Board::isPlayable() { return (!handlingPlayerTurn && !handlingTileEffects && !handlingPiecePromotion && !handlingStateUpdate); }

void Board::queueMove(Move move) {
    Piece* animatingPiece = getTile(move.from)->getPiece();

    //TODO: need to change the animation for things like conveyor belts / portals
    // The place to do it is here

    switch (move.type) {
        case MoveType::CONVEYOR_MOVE: {
            cout << "CONVEYOR MOVE FOUND!" << endl;
            raylib::Vector3 fromPos = getIsoPositionAtCell(move.from);
            raylib::Vector3 toPos = getIsoPositionAtCell(move.to);

            raylib::Vector3 offset = toPos - fromPos;

            animatingPiece->playAnimation(createSlideAnimation({ 0, 0, 0 }, offset));
            break;
        }
        case MoveType::PORTAL_MOVE: {
            raylib::Vector3 fromPos = getIsoPositionAtCell(move.from);
            raylib::Vector3 toPos = getIsoPositionAtCell(move.to);

            raylib::Vector3 offset = toPos - fromPos;

            animatingPiece->playAnimation(createTeleportAnimation({ 0, 0, 0 }, offset));
            break;
        }
        default:
            animatingPiece->playAnimation(animatingPiece->createMoveAnimation(*this, move.from, move.to));
            break;
    }

    // It's debatable whether or not tiles that move the pieces should count as a piece move, but I'm going to say yes
	getTile(move.from)->getPiece()->move();

    queuedMoves.push_back(move); // Add the move to the queue
}

void Board::removeConflictingMoves() {
    // Remove conflicting moves (moves with the same destination)
    for (auto it = queuedMoves.begin(); it != queuedMoves.end(); ++it) {
        for (auto jt = next(it); jt != queuedMoves.end();) {
            if (jt->to == it->to) {
                getPiece(jt->from)->removeAnimation();
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
                auto blockingMove = find_if(queuedMoves.begin(), queuedMoves.end(), [it](const Move& other) {
                    return (other.from == it->to) && (other.to != it->to);
                    });

                if (blockingMove == queuedMoves.end()) {
                    getPiece(it->from)->removeAnimation();
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
	cout << "Executing queued moves" << endl;
    for (auto& move : queuedMoves) {
        Piece* animatingPiece = getTile(move.from)->getPiece();

        if (animatingPiece) {
			animatingPiece->removeAnimation(); // Remove the moving animation from the piece
        } else {
            cout << "ERROR: cannot find piece at location: " << move.from.rank << ", " << move.from.file << endl;
        }

        // If the move is en passant, remove the overtaken piece manually
        if (move.flag.has_value() && move.flag.value() == MoveFlag::EN_PASSANT) {
            Piece* overtakenPiece = getTile(getEnPassantableCell())->removePiece();

            // TODO: do something with this piece here so it doesn't cause a memory leak
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

void Board::promotePieces(int player) {
    cout << "Promoting pieces..." << endl;

    int piecesPromoted = 0;
    // Add all promotions
    for (int file = 0; file < 8; file++) {
        int rank = player == 1 ? 7 : 0;
        Cell promotionCell = Cell(rank, file); // Cells that a white pawn can get promoted on

        Piece* promotionPiece = getPiece(promotionCell);

        if (promotionPiece && promotionPiece->getPlayer() == player && promotionPiece->getType() == PieceType::PAWN) {
            promotions.push(promotionCell);
			piecesPromoted++;
        }
    }
}

bool Board::hasPromotion() { return !promotions.empty(); }

Cell Board::getPromotionCell() {
    if (promotions.empty()) throw runtime_error("promotions is empty!");

    Cell cell = promotions.front();
    promotions.pop();

    return cell;
}

bool Board::hasEnPassantableCell() { return enPassantableCell.has_value(); }

Cell Board::getEnPassantableCell() { return enPassantableCell.value(); }

void Board::setEnPassantableCell(Cell cell) { enPassantableCell = cell; }

void Board::clearEnPassantableCell() { enPassantableCell.reset(); }

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

Cell Board::getCell(Tile* tile) {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            if (tiles[rank][file] == tile) {
                return Cell(rank, file);
            }
        }
    }

    return Cell(-1, -1);
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

Move Board::getMove(Cell pieceCell, Cell moveCell) {
	Piece* piece = getPiece(pieceCell);

    if (!piece) throw runtime_error("Unable to find piece.");

    // Get all legal moves for this piece
	vector<Move> moves = piece->getLegalMoves(*this);

    // Find the one that matches the move cell and return it (using find function)
	auto it = find_if(moves.begin(), moves.end(), [&](const Move& move) { return move.to == moveCell; });

	if (it != moves.end()) return *it; // Return the found move

	throw runtime_error("Unable to find move.");
}


vector<Move> Board::getAllLegalMoves(int player) {
    vector<Move> allLegalMoves;

    vector<Cell> pieceLocations = getPlayersPieces(player);

    for (Cell& pieceLocation : pieceLocations) {
        Tile* tile = getTile(pieceLocation);

        Piece* piece = tile->getPiece();

        vector<Move> pieceMoves = piece->getLegalMoves(*this);

        for (Move& move : pieceMoves) {
            allLegalMoves.push_back(move);
        }
    }

    return allLegalMoves;
}

vector<Cell> Board::getPlayersPieces(int player) {
    vector<Cell> locations;

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
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
vector<Cell> Board::getPlayersPiecesOfType(int player) {
    vector<Cell> locations;

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
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
    vector<Cell> kings = getPlayersPiecesOfType<King>(player);

    if (kings.empty()) return false; // Should not happen in a normal game

    Cell kingsPosition = kings[0];

    // Get all the possible moves of the opposing player's pieces
    vector<Cell> opponentPiecesLocations = getPlayersPieces((player % 2) + 1);

    for (Cell& location : opponentPiecesLocations) {
        Tile* tile = getTile(location);

        Piece* piece = tile->getPiece();

        vector<Move> moves = piece->getMoves(*this);

        for (Move& move : moves) {
            if (move.to == kingsPosition) return true;
        }
    }

    return false;
}

bool Board::canMove(int player) {
    vector<Cell> playerPieceLocations = getPlayersPieces(player);

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
                changeTile(rank, i, new ConveyorTile(atlas, RIGHT));
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

template vector<Tile*> Board::getTilesOfType<PortalTile>();


template vector<Cell> Board::getPlayersPiecesOfType<Rook>(int player);