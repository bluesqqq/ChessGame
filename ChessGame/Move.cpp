#include "Move.h"

Move::Move(Tile* to, Tile* from, bool canOvertake, Animation animation) : to(to), from(from), canOvertake(canOvertake), animation(animation) {}

bool Move::overtakes() {
	return (canOvertake && to->hasPiece());
}