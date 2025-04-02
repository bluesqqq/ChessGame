#pragma once

#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include "tile.h"
#include "include/raylib-cpp.hpp"
#include "easing.h"

struct Keyframe {
	double time;

	raylib::Vector3 position;

	EasingType easing;

	Keyframe(double time, raylib::Vector3 position, EasingType easing = EasingType::NONE);
};

struct Animation {
    double currentTime = 0;
	double startTime = 0;
    std::vector<Keyframe> keyframes;
	Animation(std::vector<Keyframe> keyframes);

	void startAnimation();

	raylib::Vector3 getPositionAtTime(double time);

	raylib::Vector3 getPosition();

	bool ended();
};

Animation createSlideAnimation(raylib::Vector3 start, raylib::Vector3 end);

Animation createPickAndPlaceAnimation(raylib::Vector3 start, raylib::Vector3 end);

Animation createTeleportAnimation(raylib::Vector3 start, raylib::Vector3 end);

#endif