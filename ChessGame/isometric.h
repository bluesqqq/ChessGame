#ifndef ISOMETRIC_H
#define ISOMETRIC_H

#include "include/raylib-cpp.hpp"
#include "config.h"

// Declare the function here, but don't define it.
Vector2 IsoToScreen(float isoX, float isoY, float isoZ);
Vector2 IsoToScreen(Vector3 position);
Vector3 ScreenToISO(Vector2 screenPosition, float targetZ);
Vector3 ScreenToISOFloat(Vector2 screenPosition, float targetZ);

Vector2 CursorToISO(Camera2D camera);

#endif
