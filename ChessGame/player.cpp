#include "player.h"
#include <iostream>

using namespace std;

Player::Player(string name) : name(name) {
	discardedPieces = vector<Piece*>();
}

string Player::getName() {
	return name;
}

bool Player::hasMove() {
	return (nextMove.has_value());
}

Move Player::getMove() {
	if (hasMove()) {
		Move tempMove = nextMove.value();
		nextMove = nullopt;
		return tempMove;
	}

	throw runtime_error("Player has no move.");
}

void Player::setMove(Move move) {
	nextMove = move;
}

vector<Piece*> Player::getDiscardedPieces() {
	return discardedPieces;
}

void Player::addDiscardedPiece(Piece* piece) {
	discardedPieces.push_back(piece);
}
