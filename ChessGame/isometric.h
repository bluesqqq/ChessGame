#pragma once
#ifndef ISOMETRIC_H
#define ISOMETRIC_H

#include "raylib.h">
#include "config.h"

Vector2 IsoToScreen(float isoX, float isoY, float isoZ)
{
    Vector2 screenPos;

    screenPos.x = (isoX - isoY) * (TILE_WIDTH / 2.0f);
    screenPos.y = (isoX + isoY) * (TILE_HEIGHT / 2.0f) - (isoZ * TILE_HEIGHT);

    return screenPos;
}

Vector2 ScreenToISO(Vector2 screenPosition)
{
    Vector2 isoPos;

    isoPos.x = (screenPosition.x / (TILE_WIDTH / 2.0f) + screenPosition.y / (TILE_HEIGHT / 2.0f)) / 2.0f;
    isoPos.y = (screenPosition.y / (TILE_HEIGHT / 2.0f) - screenPosition.x / (TILE_WIDTH / 2.0f)) / 2.0f;

    return isoPos;
}

#endif