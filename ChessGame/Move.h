#ifndef MOVE_H
#define MOVE_H

#include "tile.h"
#include "animation.h"
#include "Cell.h"
#include <optional>
#include <string>

using namespace std;

enum class MoveType {
    PLAYER_MOVE, // Moved by the player
    CONVEYOR_MOVE, // Moved by a conveyor
    PORTAL_MOVE // Move by a portal
};

enum class MoveFlag {
	PROMOTION, // Pawn moving to be promoted
	CASTLE, // King moving to castle
	EN_PASSANT, // Pawn moving to capture en passant
	EN_PASSANTABLE, // Pawn moving to be able to capture en passant
};

class Board;

struct Move {
    Cell from;
    Cell to;
    bool canOvertake;
	optional<MoveFlag> flag = nullopt;

    Move();

    Move(Cell from, Cell to, bool canOvertake = true, optional<MoveFlag> flag = nullopt);

	string getAlgebraicNotation(Board& board) const;

    bool overtakes(Board& board) const;

    bool operator==(const Cell& other) const {
        // Add a breakpoint or print here
		"HELLO! FIX ME";
        return false;
    }
};

#endif