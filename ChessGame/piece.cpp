#include "piece.h"
#include "board.h"

Piece::Piece(Texture2D* texture, int player, string name, Rectangle source) : atlas(texture), player(player), name(name), source(source) {}
Piece::~Piece() {}

void Piece::draw(int x, int y, float z, bool hidden) {
    if (hidden) opacity = Lerp(opacity, 0.4f, 0.25f);
    else opacity = Lerp(opacity, 1.0f, 0.25f);

    Vector2 position = IsoToScreen(x, y, z - 1 + (source.height / TILE_HEIGHT));
    DrawTextureRec(*atlas, source, position, Fade(getColor(), opacity));
}

void Piece::drawIcon(int x, int y) {
    Vector2 position = { x, y };
    DrawTextureRec(*atlas, source, position, getColor());
}

vector<pair<int, int>> Piece::getValidMoves(int x, int y, Board& board) { return {}; }

vector<pair<int, int>> Piece::getLegalMoves(int x, int y, Board& board) {
    vector<pair<int, int>> validMoves = getValidMoves(x, y, board);
    vector<pair<int, int>> legalMoves;

    for (pair<int, int> move : validMoves) {
        // This works, is it the best solution? probably not, but it works

        // Save current state
        Tile* originalTile = board.getTile(move.first, move.second);
        Piece* capturedPiece = originalTile->getPiece(); // Store if a piece is there

        // Move the piece temporarily
        board.setTile(move.first, move.second, board.getTile(x, y));
        board.setTile(x, y, nullptr);

        // Check if the player's king is in check
        if (!board.isInCheck(player)) {
            legalMoves.push_back(move);
        }

        // Undo the move
        board.setTile(x, y, board.getTile(move.first, move.second)); // Move piece back
        board.setTile(move.first, move.second, originalTile); // Restore original tile
        originalTile->setPiece(capturedPiece); // Restore captured piece if any
    }


    return legalMoves;
}




bool Piece::isValidMove(int x, int y, Board& board, int moveX, int moveY)
{
    vector<pair<int, int>> validMove = getValidMoves(x, y, board);

    std::pair<int, int> current_pair = { moveX, moveY };
    auto it = std::find(validMove.begin(), validMove.end(), current_pair);

    if (it != validMove.end())
    {
        return true;
    }

    return false;
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

Pawn::Pawn(Texture2D* texture, int player) : Piece(texture, player, "Pawn", { 4 * TILE_SIZE, 1 * TILE_SIZE, TILE_SIZE, TILE_SIZE }) {}

vector<pair<int, int>> Pawn::getValidMoves(int x, int y, Board& board) {
    vector<pair<int, int>> moves;
    int direction = (player == 1 ? 1 : -1);

    if (board.getTile(x, y + direction) && !board.getTile(x, y + direction)->hasPiece())
        moves.emplace_back(x, y + direction);

    // Can move two squares on the first move
    if (this->moves == 0 && board.getTile(x, y + direction * 2) && !board.getTile(x, y + direction * 2)->hasPiece() && !board.getTile(x, y + direction)->hasPiece())
        moves.emplace_back(x, y + direction * 2);

    if (board.getTile(x - 1, y + direction) && board.getTile(x - 1, y + direction)->hasPiece() && board.getTile(x - 1, y + direction)->getPiece()->getPlayer() != player)
        moves.emplace_back(x - 1, y + direction);

    if (board.getTile(x + 1, y + direction) && board.getTile(x + 1, y + direction)->hasPiece() && board.getTile(x + 1, y + direction)->getPiece()->getPlayer() != player)
        moves.emplace_back(x + 1, y + direction);

    return moves;
}

Knight::Knight(Texture2D* texture, int player) : Piece(texture, player, "Knight", { 4 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, TILE_SIZE }) {}

vector<pair<int, int>> Knight::getValidMoves(int x, int y, Board& board) {
    vector<pair<int, int>> moves;
    const vector<pair<int, int>> offsets = { {2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2} };

    for (auto offset : offsets) {
        int _x = x + offset.first;
        int _y = y + offset.second;

        if (board.getTile(_x, _y)) {
            Piece* piece = board.getTile(_x, _y)->getPiece();
            if (!piece || piece->getPlayer() != player) {
                moves.emplace_back(_x, _y);
            }
        }
    }
    return moves;
}

Bishop::Bishop(Texture2D* texture, int player) : Piece(texture, player, "Bishop", { 5 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, TILE_SIZE }) {}

vector<pair<int, int>> Bishop::getValidMoves(int x, int y, Board& board) {
    vector<pair<int, int>> moves;
    const vector<pair<int, int>> directions = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };

    for (auto direction : directions) {
        int _x = x, _y = y;
        while (board.getTile(_x + direction.first, _y + direction.second)) {
            _x += direction.first;
            _y += direction.second;

            Piece* piece = board.getTile(_x, _y)->getPiece();
            if (!piece) {
                moves.emplace_back(_x, _y);
            }
            else if (piece->getPlayer() != player) {
                moves.emplace_back(_x, _y);
                break;
            }
            else break;
        }
    }
    return moves;
}

Rook::Rook(Texture2D* texture, int player) : Piece(texture, player, "Rook", { 5 * TILE_SIZE, 1 * TILE_SIZE, TILE_SIZE, TILE_SIZE }) {}

vector<pair<int, int>> Rook::getValidMoves(int x, int y, Board& board) {
    vector<pair<int, int>> moves;
    const vector<pair<int, int>> directions = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };

    for (auto direction : directions) {
        int _x = x, _y = y;
        while (board.getTile(_x + direction.first, _y + direction.second)) {
            _x += direction.first;
            _y += direction.second;

            Piece* piece = board.getTile(_x, _y)->getPiece();
            if (!piece) {
                moves.emplace_back(_x, _y);
            }
            else if (piece->getPlayer() != player) {
                moves.emplace_back(_x, _y);
                break;
            }
            else break;
        }
    }
    return moves;
}

Queen::Queen(Texture2D* texture, int player) : Piece(texture, player, "Queen", { 6 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, TILE_SIZE * 2 }) {}

vector<pair<int, int>> Queen::getValidMoves(int x, int y, Board& board) {
    vector<pair<int, int>> moves;
    const vector<pair<int, int>> directions = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1}, {1, 0}, {-1, 0}, {0, 1}, {0, -1} };

    for (auto direction : directions) {
        int _x = x, _y = y;
        while (board.getTile(_x + direction.first, _y + direction.second)) {
            _x += direction.first;
            _y += direction.second;

            Piece* piece = board.getTile(_x, _y)->getPiece();
            if (!piece) {
                moves.emplace_back(_x, _y);
            }
            else if (piece->getPlayer() != player) {
                moves.emplace_back(_x, _y);
                break;
            }
            else break;
        }
    }
    return moves;
}

King::King(Texture2D* texture, int player) : Piece(texture, player, "King", { 7 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, TILE_SIZE * 2 }) {}

vector<pair<int, int>> King::getValidMoves(int x, int y, Board& board) {
    vector<pair<int, int>> moves;

    // Check a 3x3 box with the King in the center (except for out-of-bounds)
    for (int _x = max(x - 1, 0); _x <= min(x + 1, 7); _x++)
    {
        for (int _y = max(y - 1, 0); _y <= min(y + 1, 7); _y++)
        {
            // Get the tile at the current location
            Tile* tile = board.getTile(_x, _y);

            // Get the piece on the current tile
            Piece* piece = tile->getPiece();

            // Add the move if there is no piece, or if the piece is the opponents
            if (!piece || piece->getPlayer() != player)
            {
                moves.emplace_back(_x, _y);
            }
        }
    }

    return moves;
}