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
    Animation animation;

    Move(Cell to, Cell from, bool canOvertake, Animation animation);

    bool overtakes(Board& board);
};

#endif