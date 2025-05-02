#ifndef MOVE_H
#define MOVE_H

#include "tile.h"
#include "animation.h"
#include "Cell.h"

enum class MoveType {
    PLAYER_MOVE, // Moved by the player
    CONVEYOR_MOVE, // Moved by a conveyor
    PORTAL_MOVE // Move by a portal
};

class Board;

struct Move {
    Cell to;
    Cell from;
    bool canOvertake;

    Move(Cell to, Cell from, bool canOvertake);

    bool overtakes(Board& board);
};

#endif