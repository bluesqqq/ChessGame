#include "Background.h"
#include <iostream>
#include "isometric.h"

float random_float(float min, float max) {
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(min, max);
	return dis(gen);
}

Star::Star() {
	position = raylib::Vector3(random_float(-5, 15), -10, 0);
	speed = random_float(0.01f, 0.1f);
	radius = random_float(0.5f, 3.0f);
	color = WHITE;
}

void SpaceBackground::draw() {
	ClearBackground(BLACK);

	for (Star& star : stars) {
		Vector2 screenPos = IsoToScreen(star.position);
		DrawCircle(screenPos.x, screenPos.y, star.radius, star.color);
	}
}

void SpaceBackground::update() {
	if (rand() % 3 == 0) {
		stars.push_back(Star());
	}

	for (Star& star : stars) {
		star.position.y += star.speed;
	}

	// Remove any stars that have gone off the screen
	stars.erase(remove_if(stars.begin(), stars.end(), [](Star& a) { return a.position.y > 20; }), stars.end());
}


void SkyBackground::draw() {
	ClearBackground(Color{ 204, 230, 255, 255 });
}

void SkyBackground::update() {

}