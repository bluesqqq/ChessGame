#include "piece.h"
#include "board.h"
#include "textures.h"

Piece::Piece(raylib::Texture2D* texture, int player, PieceType pieceType) : atlas(texture), player(player), pieceType(pieceType) {

}

Piece::~Piece() {}

void Piece::draw(RenderQueue& renderQueue, float x, float y, float z, bool hidden) {
	if (hasAnimation()) {
		setOffset(animation->getPosition());
    }
    else {
		setOffset({ 0.0f, 0.0f, 0.0f });
    }

    if (hidden) opacity = Lerp(opacity, 0.4f, 0.25f);
    else opacity = Lerp(opacity, 1.0f, 0.25f);

    if (frozen > 0) {
        renderQueue.addSpriteObject(SpriteObject(raylib::Vector3(x, y, z) + offset, atlas, frozenSpriteRect, WHITE, opacity));
    } else {
        renderQueue.addSpriteObject(SpriteObject(raylib::Vector3(x, y, z) + offset, atlas, spriteRect, getColor(), opacity));
    }
}

void Piece::drawIcon(int x, int y) {
    Vector2 position = { x, y };
    DrawTextureRec(*atlas, spriteRect, position, getColor());
}

bool Piece::hasAnimation() {
    return animation.has_value();
}

void Piece::playAnimation(Animation anim) {
    animation = anim;
    animation.value().play();
}

bool Piece::animationFinished() {
    if (!hasAnimation()) return true;

    if (animation->ended()) return true;

    return false;
}

void Piece::removeAnimation() { animation.reset(); }

void Piece::setOffset(raylib::Vector3 offset) {
	this->offset = offset;
}

Animation Piece::createMoveAnimation(Board& board, Cell from, Cell to) {
	raylib::Vector3 fromPos = board.getIsoPositionAtCell(from);
    raylib::Vector3 toPos = board.getIsoPositionAtCell(to);

	raylib::Vector3 offset = toPos - fromPos;

    return createPickAndPlaceAnimation({ 0, 0, 0 }, offset);
}

void Piece::update() {

}

int Piece::getNumberOfMoves() {
    return moves;
}

void Piece::updateState() {
    if (frozen > 0) frozen--;
}

vector<Move> Piece::getLegalMoves(Board& board) {
    if (getImmobile()) return {}; // If the piece is immobile, return an empty list

    vector<Move> pseudoLegalMoves = getMoves(board);

	if (pseudoLegalMoves.empty()) return {}; // If there are no moves, return an empty list

    vector<Move> legalMoves;

    Cell pieceCell = board.getCell(this);

    Tile* originalTile = board.getTile(pieceCell);

    for (Move& move : pseudoLegalMoves) {
        // Save current state
        Tile* moveToTile     = board.getTile(move.to);

        // Remove the captured piece from the tile
        Piece* capturedPiece = moveToTile->removePiece();

        // Set the move to tile's piece to the original tile's piece
        moveToTile->setPiece(originalTile->removePiece());

        // Check if the player's king is in check after making this move
        if (!board.isInCheck(player)) {
            legalMoves.push_back(move);
        }

        // Undo the move
        originalTile->setPiece(moveToTile->removePiece());

        // If there was a piece captured in the process, place it back
        if (capturedPiece) {
            moveToTile->setPiece(capturedPiece);
        }
    }

    return legalMoves;
}

bool Piece::isLegalMove(Board& board, Cell move) {
    vector<Move> legalMoves = getLegalMoves(board);

    if (legalMoves.empty()) return false;

    return any_of(legalMoves.begin(), legalMoves.end(), [&](const Move& m) {
        return m.to == move;
    });
}

Color Piece::getColor() const {
    return (player == 1) ? WHITE : GRAY;
}

int Piece::getPlayer() {
    return player;
}

void Piece::move() {
    moves++;
}

void Piece::setFrozen(int frozenTurns) {
    frozen = frozenTurns;
}

bool Piece::getImmobile() {
    return frozen > 0;
}

bool Piece::isSelectable() {
    return !getImmobile();
}

string Piece::getName() {
    vector<string> names{
        "None",
        "Pawn",
        "Knight",
        "Bishop",
        "Rook",
        "Queen",
        "King"
    };

    return names[static_cast<int>(pieceType)];
}

PieceType Piece::getType() {
    return pieceType;
}


Pawn::Pawn(raylib::Texture2D* texture, int player) : Piece(texture, player, PieceType::PAWN) {
    spriteRect = pieceSprites[SPRITE_PAWN].toSpriteRect();
    frozenSpriteRect = pieceSprites[SPRITE_PAWN_FROZEN].toSpriteRect();
}

vector<Move> Pawn::getMoves(Board& board) {
    vector<Move> moves;

    Cell pieceCell = board.getCell(this);

    int rank = pieceCell.rank;
    int file = pieceCell.file;

    int direction = getDirection();

    Tile* forwardTile = board.getTile(Cell(rank + direction, file));
    Tile* doubleForwardTile = board.getTile(Cell(rank + direction * 2, file));

    // Move 1 forward
    if (forwardTile && !forwardTile->hasPiece()) {
        moves.emplace_back(Move(pieceCell, Cell(rank + direction, file)));

        // Can move two squares on the first move
        if (forwardTile->isPassable() && this->moves == 0 && doubleForwardTile && !doubleForwardTile->hasPiece()) {
            moves.emplace_back(Move(pieceCell, Cell(rank + direction * 2, file), true, MoveFlag::EN_PASSANTABLE));
        }
    }

    Tile* leftTile = board.getTile(Cell(rank + direction, file - 1));
    Tile* rightTile = board.getTile(Cell(rank + direction, file + 1));

    // Capture up and left
    if (leftTile && leftTile->hasPiece() && leftTile->getPiece()->getPlayer() != player)
        moves.emplace_back(Move(pieceCell, Cell(rank + direction, file - 1)));

    // Capture up and right
    if (rightTile && rightTile->hasPiece() && rightTile->getPiece()->getPlayer() != player)
        moves.emplace_back(Move(pieceCell, Cell(rank + direction, file + 1)));

    // En passant
    if (board.hasEnPassantableCell()) {
		Cell enPassantableCell = board.getEnPassantableCell();

		if (enPassantableCell == Cell(rank, file - 1)) { // En passant left
			moves.emplace_back(Move(pieceCell, Cell(rank + direction, file - 1), false, MoveFlag::EN_PASSANT));
		} else if (enPassantableCell == Cell(rank, file + 1)) { // En passant right
			moves.emplace_back(Move(pieceCell, Cell(rank + direction, file + 1), false, MoveFlag::EN_PASSANT));
		}
    }

    return moves;
}

string Pawn::getAlgebraicNotation() { return "P"; }

Knight::Knight(raylib::Texture2D* texture, int player) : Piece(texture, player, PieceType::KNIGHT) {
    spriteRect = pieceSprites[SPRITE_KNIGHT].toSpriteRect();
    frozenSpriteRect = pieceSprites[SPRITE_KNIGHT_FROZEN].toSpriteRect();
}

vector<Move> Knight::getMoves(Board& board) {
    vector<Move> moves;

    Cell pieceCell = board.getCell(this);

    const vector<Cell> offsets = { {2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2} };

    for (const Cell& offset : offsets) {
        const Cell _cell = pieceCell + offset;

        Tile* tile = board.getTile(_cell);

        if (tile) {
            Piece* piece = tile->getPiece();
            if (!piece || piece->getPlayer() != player) {
                moves.emplace_back(Move(pieceCell, _cell));
            }
        }
    }

    return moves;
}

string Knight::getAlgebraicNotation() { return "N"; }

Bishop::Bishop(raylib::Texture2D* texture, int player) : Piece(texture, player, PieceType::BISHOP) {
    spriteRect = pieceSprites[SPRITE_BISHOP].toSpriteRect();
    frozenSpriteRect = pieceSprites[SPRITE_BISHOP_FROZEN].toSpriteRect();
}

vector<Move> Bishop::getMoves(Board& board) {
    vector<Move> moves;

    Cell pieceCell = board.getCell(this);

    const vector<Cell> directions = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };

    for (const Cell& direction : directions) {
        Cell _cell = pieceCell;

        while (board.getTile(_cell + direction)) {
            _cell += direction;

            Tile* tile = board.getTile(_cell);

            if (!tile) break; // If theres no tile, piece cannot traverse farther

            Piece* piece = tile->getPiece();

            if (!piece) {
                moves.emplace_back(Move(pieceCell, _cell));
            } else if (piece->getPlayer() != player) {
                moves.emplace_back(Move(pieceCell, _cell));
                break;
            } else break;

            if (!tile->isPassable()) break;
        }
    }
    return moves;
}

Animation Bishop::createMoveAnimation(Board& board, Cell from, Cell to) {
    raylib::Vector3 fromPos = board.getIsoPositionAtCell(from);
    raylib::Vector3 toPos = board.getIsoPositionAtCell(to);

    raylib::Vector3 offset = toPos - fromPos;

    return createSlideAnimation({ 0, 0, 0 }, offset);
}

string Bishop::getAlgebraicNotation() { return "B"; }

Rook::Rook(raylib::Texture2D* texture, int player) : Piece(texture, player, PieceType::ROOK) {
    spriteRect = pieceSprites[SPRITE_ROOK].toSpriteRect();
    frozenSpriteRect = pieceSprites[SPRITE_ROOK_FROZEN].toSpriteRect();
}

vector<Move> Rook::getMoves(Board& board) {
    vector<Move> moves;

    Cell pieceCell = board.getCell(this);

    const vector<Cell> directions = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };

    for (const Cell& direction : directions) {
        Cell _cell = pieceCell;

        while (board.getTile(_cell + direction)) {
            _cell += direction;

            Tile* tile = board.getTile(_cell);

            if (!tile) break; // If theres no tile, piece cannot traverse farther

            Piece* piece = tile->getPiece();

            if (!piece) {
                moves.emplace_back(Move(pieceCell, _cell));
            }
            else if (piece->getPlayer() != player) {
                moves.emplace_back(Move(pieceCell, _cell));
                break;
            }
            else break;

            if (!tile->isPassable()) break;
        }
    }

    return moves;
}

Animation Rook::createMoveAnimation(Board& board, Cell from, Cell to) {
    raylib::Vector3 fromPos = board.getIsoPositionAtCell(from);
    raylib::Vector3 toPos = board.getIsoPositionAtCell(to);

    raylib::Vector3 offset = toPos - fromPos;

    return createSlideAnimation({ 0, 0, 0 }, offset);
}

string Rook::getAlgebraicNotation() { return "R"; }

Queen::Queen(raylib::Texture2D* texture, int player) : Piece(texture, player, PieceType::QUEEN) {
    spriteRect = pieceSprites[SPRITE_QUEEN].toSpriteRect();
    frozenSpriteRect = pieceSprites[SPRITE_QUEEN_FROZEN].toSpriteRect();
}

vector<Move> Queen::getMoves(Board& board) {
    vector<Move> moves;

    Cell pieceCell = board.getCell(this);

    const vector<Cell> directions = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1}, {1, 0}, {-1, 0}, {0, 1}, {0, -1} };

    for (const Cell& direction : directions) {
        Cell _cell = pieceCell;

        while (board.getTile(_cell + direction)) {
            _cell += direction;

            Tile* tile = board.getTile(_cell);

            if (!tile) break; // If theres no tile, piece cannot traverse farther

            Piece* piece = tile->getPiece();

            if (!piece) {
                moves.emplace_back(Move(pieceCell, _cell));
            }
            else if (piece->getPlayer() != player) {
                moves.emplace_back(Move(pieceCell, _cell));
                break;
            }
            else break;

            if (!tile->isPassable()) break;
        }
    }

    return moves;
}

Animation Queen::createMoveAnimation(Board& board, Cell from, Cell to) {
    raylib::Vector3 fromPos = board.getIsoPositionAtCell(from);
    raylib::Vector3 toPos = board.getIsoPositionAtCell(to);

    raylib::Vector3 offset = toPos - fromPos;

    return createSlideAnimation({ 0, 0, 0 }, offset);
}

string Queen::getAlgebraicNotation() { return "Q"; }

King::King(raylib::Texture2D* texture, int player) : Piece(texture, player, PieceType::KING) {
    spriteRect = pieceSprites[SPRITE_KING].toSpriteRect();
    frozenSpriteRect = pieceSprites[SPRITE_KING_FROZEN].toSpriteRect();
}

vector<Move> King::getMoves(Board& board) {
    vector<Move> moves;

    Cell pieceCell = board.getCell(this);

    int rank = pieceCell.rank;
    int file = pieceCell.file;

    // Check a 3x3 box with the King in the center (except for out-of-bounds)
    for (int _rank = max(rank - 1, 0); _rank <= min(rank + 1, 7); _rank++) {
        for (int _file = max(file - 1, 0); _file <= min(file + 1, 7); _file++) {
            // Get the tile at the current location
            Tile* tile = board.getTile(Cell(_rank, _file));

            if (!tile) continue; // Without this line everyting crashes fatally

            // Get the piece on the current tile
            Piece* piece = tile->getPiece();

            // Add the move if there is no piece, or if the piece is the opponents
            if (!piece || piece->getPlayer() != player) {
                moves.emplace_back(Move(pieceCell, Cell(_rank, _file)));
            }
        }
    }

    // CASTLING
    if (getNumberOfMoves() <= 0) { // King has not moved
        vector<Cell> rookCells = board.getPlayersPiecesOfType<Rook>(player);

        for (Cell& rookCell : rookCells) {
            if (rookCell.rank == pieceCell.rank) { // Same rank as king
                Tile* rookTile = board.getTile(rookCell);
                Piece* rookPiece = rookTile->getPiece();

                if (rookPiece->getNumberOfMoves() <= 0) { // Rook has not moved
                    bool blockingPiece = false;

                    int start = min(pieceCell.file, rookCell.file) + 1;
                    int end = max(pieceCell.file, rookCell.file);

                    for (int i = start; i < end; i++) {
                        Tile* tile = board.getTile(Cell(pieceCell.rank, i));
                        if (tile->hasPiece()) {
                            blockingPiece = true;
                            break;
                        }
                    }

                    if (!blockingPiece) {
                        int direction = (rookCell.file < pieceCell.file) ? -1 : 1;
                        // Add the tile the king would move to as a castling move
                        moves.emplace_back(Move(pieceCell, pieceCell + Cell(0, direction * 2), false, MoveFlag::CASTLE));
                    }
                }
            }
        }
    }


    return moves;
}

string King::getAlgebraicNotation() { return "K"; }