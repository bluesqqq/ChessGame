#include "Move.h"
#include "board.h"

Move::Move(Cell from, Cell to, bool canOvertake, optional<MoveFlag> flag) : from(from), to(to), canOvertake(canOvertake), flag(flag) {}

bool Move::overtakes(Board& board) {
	return (canOvertake && board.getTile(to)->hasPiece());
}