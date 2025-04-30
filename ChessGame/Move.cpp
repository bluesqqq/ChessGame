#include "Move.h"
#include "board.h"

Move::Move(Cell to, Cell from, bool canOvertake) : to(to), from(from), canOvertake(canOvertake) {}

bool Move::overtakes(Board& board) {
	return (canOvertake && board.getTile(to)->hasPiece());
}