#include "Move.h"
#include "board.h"

Move::Move() : from(-1, -1), to(-1, -1), canOvertake(false), flag(nullopt) {}

Move::Move(Cell from, Cell to, bool canOvertake, optional<MoveFlag> flag, MoveType type) : from(from), to(to), canOvertake(canOvertake), flag(flag), type(type) {}

string Move::getAlgebraicNotation(Board& board) const {
	Piece* piece = board.getPiece(from);

	if (!piece) return "IMPOSSIBLE MOVE"; // No piece at the from cell

	// Castling
	if (flag.has_value() && flag.value() == MoveFlag::CASTLE) {
		if (from.file > to.file) {
			return "O-O-O"; // Queenside castle
		} else {
			return "O-O"; // Kingside castle
		}
	}

	// Overtaking 
	if (overtakes(board)) {
		if (piece->getType() == PieceType::PAWN) {
			string enPassant = (flag.has_value() && flag.value() == MoveFlag::EN_PASSANT) ? " e.p." : "";
			char f = ('a' + from.file);
			return f + "x" + to.getAlgebraicNotation() + enPassant;
		}

		return piece->getAlgebraicNotation() + "x" + to.getAlgebraicNotation();
	}

	// Normal
	return piece->getAlgebraicNotation() + to.getAlgebraicNotation();
}

bool Move::overtakes(Board& board) const {
	return (canOvertake && board.getTile(to)->hasPiece());
}