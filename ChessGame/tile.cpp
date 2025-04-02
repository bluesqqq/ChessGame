#include "tile.h"
#include "piece.h"
#include "board.h"

Tile::Tile(int lifetime) : lifetime(lifetime) { }

void Tile::update() {
	if (currentPiece) {
		currentPiece->update();
	}
}

int Tile::getLifetime() {
    return lifetime;
}

void Tile::setPiece(Piece* piece) {
    currentPiece = piece;
}

void Tile::queuePiece(Piece* piece) {
    queuedPiece = piece;
}

Piece* Tile::dequeuePiece() {
    
    Piece* removedPiece = nullptr; 

    if (queuedPiece) {
        removedPiece = removePiece();

        currentPiece = queuedPiece;
        queuedPiece = nullptr;
    }

    return removedPiece;
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

