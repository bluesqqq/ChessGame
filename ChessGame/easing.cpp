#include "easing.h"

float easeValue(float t, EasingType easing) {
	switch (easing) {
		case EasingType::EASE_IN:
			return easeIn(t);
		case EasingType::EASE_OUT:
			return easeOut(t);
		case EasingType::EASE_IN_OUT:
			return easeInOut(t);
		default:
			return t;
	}
}

float easeIn(float t) {
	return t * t;
}

float easeOut(float t) {
	return t * (2 - t);
}

float easeInOut(float t) {
	return t < 0.5 ? 2 * t * t : -1 + (4 - 2 * t) * t;
}