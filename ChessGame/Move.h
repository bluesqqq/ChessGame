#ifndef MOVE_H
#define MOVE_H

#include "tile.h"
#include "animation.h"
#include "Cell.h"

class Board;

struct Move {
    Cell to;
    Cell from;
    bool canOvertake;

    Move(Cell to, Cell from, bool canOvertake);

    bool overtakes(Board& board);
};

#endif