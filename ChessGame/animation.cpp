#include "animation.h"

Keyframe::Keyframe(double time, raylib::Vector3 position, EasingType easing) : time(time), position(position), easing(easing) {

}

Animation::Animation(std::vector<Keyframe> keyframes) : keyframes(keyframes) {

}

void Animation::startAnimation() {
	startTime = GetTime();
}

raylib::Vector3 Animation::getPositionAtTime(double time) {
	// If there are no keyframes, return a zero vector
	if (keyframes.size() == 0) {
		return { 0.0f, 0.0f, 0.0f };
	}

	// If there is only one keyframe, return that keyframe's position
	if (keyframes.size() == 1) {
		return keyframes[0].position;
	}

	// If the time is before the first keyframe, return the first keyframe's position
	if (time < keyframes[0].time) {
		return keyframes[0].position;
	}

	// If the time is between two keyframes, interpolate between them
	for (int i = 0; i < keyframes.size() - 1; i++) {
		// Check if the time is between the current keyframe and the next keyframe
		if (time >= keyframes[i].time && time < keyframes[i + 1].time) {
			float timeDiff = keyframes[i + 1].time - keyframes[i].time;
			float timeInto = time - keyframes[i].time;
			float ratio = timeInto / timeDiff;

			ratio = easeValue(ratio, keyframes[i].easing);

			return keyframes[i].position.Lerp(keyframes[i + 1].position, ratio);
		}
	}

	// if the time is after the last keyframe, return the last keyframe's position
	return keyframes[keyframes.size() - 1].position;
}

raylib::Vector3 Animation::getPosition() {
	return getPositionAtTime(currentTime - startTime);
}

bool Animation::ended() {
	// this here check
	return (currentTime - startTime) >= keyframes[keyframes.size() - 1].time;
}

Animation createSlideAnimation(raylib::Vector3 start, raylib::Vector3 end) {
	return Animation({
		Keyframe(0, start), 
		Keyframe(1, end)
	});
}

Animation createPickAndPlaceAnimation(raylib::Vector3 start, raylib::Vector3 end) {
	return Animation({
		Keyframe(0, start, EASE_OUT), // Starting Position
		Keyframe(0.5, start + raylib::Vector3(0, 0, 2), EASE_IN_OUT), // Above Starting Position
		Keyframe(1.5, end + raylib::Vector3(0, 0, 2), EASE_IN), // Above Ending Position
		Keyframe(2, end) // Ending Position
	});
}