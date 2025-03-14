#include "isometric.h"

Vector2 IsoToScreen(float isoX, float isoY, float isoZ)
{
    Vector2 screenPos;
    screenPos.x = (isoX - isoY) * (TILE_WIDTH / 2.0f) - (TILE_WIDTH / 2.0f); // Subtract half width to center
    screenPos.y = (isoX + isoY) * (TILE_HEIGHT / 2.0f) - (isoZ * TILE_HEIGHT);
    return screenPos;
}

Vector2 ScreenToISO(Vector2 screenPosition)
{
    Vector2 isoPos;
    isoPos.x = floor((screenPosition.x / (TILE_WIDTH / 2.0f) + screenPosition.y / (TILE_HEIGHT / 2.0f)) / 2.0f);
    isoPos.y = floor((screenPosition.y / (TILE_HEIGHT / 2.0f) - screenPosition.x / (TILE_WIDTH / 2.0f)) / 2.0f);
    return isoPos;
}

Vector2 ScreenToISOFloat(Vector2 screenPosition)
{
    Vector2 isoPos;
    isoPos.x = (screenPosition.x / (TILE_WIDTH / 2.0f) + screenPosition.y / (TILE_HEIGHT / 2.0f)) / 2.0f;
    isoPos.y = (screenPosition.y / (TILE_HEIGHT / 2.0f) - screenPosition.x / (TILE_WIDTH / 2.0f)) / 2.0f;
    return isoPos;
}

Vector2 CursorToISO(Camera2D camera)
{
    raylib::Vector2 cursorPosition = GetMousePosition();

    raylib::Vector2 cursorIsoPosition = ScreenToISO(cursorPosition - camera.offset);

    return cursorIsoPosition;
}