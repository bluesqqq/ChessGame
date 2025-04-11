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
	return (nextMove != nullptr);
}

Move* Player::getMove() {
	if (hasMove()) {
		Move* tempMove = nextMove;
		nextMove = nullptr;
		return tempMove;
	}

	return nullptr;
}

void Player::setMove(Move* move) {
	cout << "setting move of player..." << endl;
	nextMove = move;
}

vector<Piece*> Player::getDiscardedPieces() {
	return discardedPieces;
}

void Player::addDiscardedPiece(Piece* piece) {
	discardedPieces.push_back(piece);
}
