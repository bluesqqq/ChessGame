#pragma once

#ifndef EASING_H
#define EASING_H

#include <cmath>

enum EasingType {
	NONE,
	EASE_IN,
	EASE_OUT,
	EASE_IN_OUT
};

float easeValue(float t, EasingType easing);

float easeIn(float t);
float easeOut(float t);
float easeInOut(float t);

#endif