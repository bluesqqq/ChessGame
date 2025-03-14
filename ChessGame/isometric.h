#pragma once
#ifndef ISOMETRIC_H
#define ISOMETRIC_H

#include "include/raylib-cpp.hpp"
#include "config.h"

// Declare the function here, but don't define it.
Vector2 IsoToScreen(float isoX, float isoY, float isoZ);
Vector2 ScreenToISO(Vector2 screenPosition);
Vector2 ScreenToISOFloat(Vector2 screenPosition);

Vector2 CursorToISO(Camera2D camera);

#endif
