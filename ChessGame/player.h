#ifndef PLAYER_H
#define PLAYER_H

#include "piece.h"
#include "Move.h"

#include <optional>

class Player {
	private:
		optional<Move> nextMove;
		string name;
		vector<Piece*> discardedPieces;

	public:
		Player(string name);

		string getName();

		bool hasMove();

		Move getMove();

		void setMove(Move move);

		vector<Piece*> getDiscardedPieces();

		void addDiscardedPiece(Piece* piece);
};

#endif