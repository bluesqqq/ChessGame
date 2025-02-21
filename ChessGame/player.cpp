#include "player.h"

Player::Player(string name) : name(name)
{
	discardedPieces = vector<Piece*>();
}

string Player::getName() {
	return name;
}

vector<Piece*> Player::getDiscardedPieces() {
	return discardedPieces;
}

void Player::addDiscardedPiece(Piece* piece) {
	discardedPieces.push_back(piece);
}