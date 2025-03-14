#include "tile.h"
#include "piece.h"
#include "board.h"

Tile::Tile(int lifetime) : lifetime(lifetime) { }

int Tile::getLifetime() {
    return lifetime;
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