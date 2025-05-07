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
	/// <summary>
	/// The time that the animation started at
	/// </summary>
	double startTime = 0;

	/// <summary>
	/// The speed of the animation
	/// </summary>
	float speed = 1.0f;

    /// <summary>
	/// The keyframes of the animation
    /// </summary>
    vector<Keyframe> keyframes;

	Animation(vector<Keyframe> keyframes);

	void startAnimation();

	void play(float speed = 1.0f);

	

	raylib::Vector3 getPositionAtTime(double time);

	raylib::Vector3 getPosition();

	bool ended();
};

Animation createInstantAnimation();

Animation createSlideAnimation(raylib::Vector3 start, raylib::Vector3 end);

Animation createPickAndPlaceAnimation(raylib::Vector3 start, raylib::Vector3 end);

Animation createTeleportAnimation(raylib::Vector3 start, raylib::Vector3 end);

#endif