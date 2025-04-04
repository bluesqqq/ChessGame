#pragma once

#include "piece.h"

class Player {
	private:
		string name;
		vector<Piece*> discardedPieces;

	public:
		Player(string name);

		string getName();

		vector<Piece*> getDiscardedPieces();

		void addDiscardedPiece(Piece* piece);
};