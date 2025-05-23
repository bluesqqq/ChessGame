#include "tile.h"
#include "piece.h"
#include "board.h"

Tile::Tile(int lifetime) : lifetime(lifetime) { }

void Tile::update() {
	if (currentPiece) {
		currentPiece->update();
	}
}

void Tile::updateState(Board& board) { if (hasPiece()) currentPiece->updateState(); }

void Tile::applyTileEffect(Board& board) { }

int Tile::getLifetime() { return lifetime; }

void Tile::setLifetime(int _lifetime) { lifetime = _lifetime; }

void Tile::setPiece(Piece* piece) { currentPiece = piece; }

void Tile::queuePiece(Piece* piece) { queuedPiece = piece; }

Piece* Tile::dequeuePiece() {
    Piece* removedPiece = nullptr; 

    if (queuedPiece) {
        removedPiece = removePiece();

        currentPiece = queuedPiece;
        queuedPiece = nullptr;
    }

    return removedPiece;
}

Piece* Tile::removePiece() {
    Piece* tempPiece = currentPiece;
    currentPiece = nullptr;
    return tempPiece;
}

bool Tile::hasPiece() { return currentPiece != nullptr; }

Piece* Tile::getPiece() { return currentPiece; }

bool Tile::isSelectable() { return true; }

bool Tile::isPassable() { return true; }