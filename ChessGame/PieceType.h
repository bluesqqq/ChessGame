#ifndef PIECETYPE_H
#define PIECETYPE_H

#include <string>

using namespace std;

enum class PieceType {
    NO_PIECE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
};

inline int getPieceValue(PieceType type) {
	switch (type) {
		case PieceType::PAWN: return 100;
		case PieceType::KNIGHT: return 320;
		case PieceType::BISHOP: return 330;
		case PieceType::ROOK: return 500;
		case PieceType::QUEEN: return 900;
		case PieceType::KING: return 0;
		default: return 0;
	}
}

inline string getPieceString(PieceType type, int8_t player = 1) {
	string pieceString = "-";
	switch (type) {
		case PieceType::PAWN: pieceString = "P"; break;
		case PieceType::KNIGHT: pieceString = "N"; break;
		case PieceType::BISHOP: pieceString = "B"; break;
		case PieceType::ROOK: pieceString = "R"; break;
		case PieceType::QUEEN: pieceString = "Q"; break;
		case PieceType::KING: pieceString = "K"; break;
	}

	if (player != 1) {
		pieceString = tolower(pieceString[0]);
	}

	return pieceString;
}

#endif