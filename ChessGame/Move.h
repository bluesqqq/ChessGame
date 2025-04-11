#ifndef MOVE_H
#define MOVE_H

#include "tile.h"
#include "animation.h"

struct Move {
    Tile* to;
    Tile* from;
    bool canOvertake;
    Animation animation;

    Move(Tile* to, Tile* from, bool canOvertake, Animation animation);
};

#endif