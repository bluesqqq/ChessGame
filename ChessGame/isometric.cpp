#include "isometric.h"

Vector2 IsoToScreen(float isoX, float isoY, float isoZ) {
    Vector2 screenPos = { 0.0f, 0.0f };
    screenPos.x = (float)((isoX - isoY) * (TILE_WIDTH / 2.0f) - (TILE_WIDTH / 2.0f)); // Subtract half width to center
    screenPos.y = (float)((isoX + isoY) * (TILE_HEIGHT / 2.0f) - (isoZ * TILE_HEIGHT));
    return screenPos;
}

Vector2 IsoToScreen(Vector3 position) {
	return IsoToScreen(position.x, position.y, position.z);
}

Vector3 ScreenToISO(Vector2 screenPosition, float targetZ) {
    // TODO: ALTER THIS BASED ON TARGETZ
    Vector3 isoPos;
    isoPos.x = floor((screenPosition.x / (TILE_WIDTH / 2.0f) + screenPosition.y / (TILE_HEIGHT / 2.0f)) / 2.0f);
    isoPos.y = floor((screenPosition.y / (TILE_HEIGHT / 2.0f) - screenPosition.x / (TILE_WIDTH / 2.0f)) / 2.0f);
    isoPos.z = targetZ;
    return isoPos;
}

Vector3 ScreenToISOFloat(Vector2 screenPosition, float targetZ) {
    Vector3 isoPos;
    isoPos.x = (screenPosition.x / (TILE_WIDTH / 2.0f) + screenPosition.y / (TILE_HEIGHT / 2.0f)) / 2.0f;
    isoPos.y = (screenPosition.y / (TILE_HEIGHT / 2.0f) - screenPosition.x / (TILE_WIDTH / 2.0f)) / 2.0f;
    isoPos.z = targetZ;
    return isoPos;
}

Vector3 CursorToISO(Camera2D camera, float targetZ) {
    raylib::Vector2 cursorPosition = GetMousePosition();

    raylib::Vector3 cursorIsoPosition = ScreenToISO(cursorPosition - camera.offset, targetZ);

    return cursorIsoPosition;
}