#include "animation.h"

Keyframe::Keyframe(double time, raylib::Vector3 position, EasingType easing) : time(time), position(position), easing(easing) {

}

Animation::Animation(vector<Keyframe> keyframes) : keyframes(keyframes) {

}

void Animation::startAnimation() {
	startTime = GetTime();
}

void Animation::play(float speed) {
	startTime = GetTime();
	this->speed = speed;
}

raylib::Vector3 Animation::getPositionAtTime(double time) {
	// If there are no keyframes, return a zero vector
	if (keyframes.size() == 0) return { 0.0f, 0.0f, 0.0f };

	// If there is only one keyframe, return that keyframe's position
	if (keyframes.size() == 1) return keyframes[0].position;

	// If the time is before the first keyframe, return the first keyframe's position
	if (time < (keyframes[0].time * speed)) return keyframes[0].position;

	// If the time is between two keyframes, interpolate between them
	for (int i = 0; i < keyframes.size() - 1; i++) {
		double thisTime = keyframes[i].time * speed;
		double nextTime = keyframes[i + 1].time * speed;

		// Check if the time is between the current keyframe and the next keyframe
		if (time >= thisTime && time < nextTime) {
			float timeDiff = nextTime - thisTime;
			float timeInto = time - thisTime;
			float ratio = timeInto / timeDiff;

			ratio = easeValue(ratio, keyframes[i].easing);

			return keyframes[i].position.Lerp(keyframes[i + 1].position, ratio);
		}
	}

	// if the time is after the last keyframe, return the last keyframe's position
	return keyframes[keyframes.size() - 1].position;
}

raylib::Vector3 Animation::getPosition() {
	double currentTime = GetTime();
	return getPositionAtTime(currentTime - startTime);
}

bool Animation::ended() {
	double currentTime = GetTime();
	// this here check
	return (currentTime - startTime) >= keyframes[keyframes.size() - 1].time * speed;
}

Animation createInstantAnimation() {
	return Animation({
	Keyframe(0, { 0.0f, 0.0f, 0.0f })
		});
}

Animation createSlideAnimation(raylib::Vector3 start, raylib::Vector3 end) {
	return Animation({
		Keyframe(0, start, EASE_IN_OUT), 
		Keyframe(0.5, end)
	});
}

Animation createPickAndPlaceAnimation(raylib::Vector3 start, raylib::Vector3 end) {
	return Animation({
		Keyframe(0, start, EASE_OUT), // Starting Position
		Keyframe(0.3, start + raylib::Vector3(0, 0, 2), EASE_IN_OUT), // Above Starting Position
		Keyframe(0.9, end + raylib::Vector3(0, 0, 2), EASE_IN), // Above Ending Position
		Keyframe(1.2, end) // Ending Position
	});
}

Animation createTeleportAnimation(raylib::Vector3 start, raylib::Vector3 end) {
	return Animation({
		Keyframe(0, start, EASE_IN), // Starting Position
		Keyframe(0.5, start + raylib::Vector3(0, 0, -1)), // Above Starting Position
		Keyframe(0.501, end + raylib::Vector3(0, 0, -1), EASE_OUT), // Above Ending Position
		Keyframe(2, end) // Ending Position
		});
}