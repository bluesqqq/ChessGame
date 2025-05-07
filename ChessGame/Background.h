#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "include/raylib-cpp.hpp"
#include <vector>
#include <random>

using namespace std;

float random_float(float min, float max);



class Background {
	public:
		virtual void draw() = 0;
		virtual void update() = 0;
};

struct Star {
	raylib::Vector3 position;
	float speed;
	int radius;
	Color color;
	Star();
};

class SpaceBackground : public Background {
	private:
		Color backgroundColor = BLACK;
		vector<Star> stars;

	public:
		void draw() override;
		void update() override;
};

class SkyBackground : public Background {
	private:
		Color backgroundColor = BLUE;

	public:
		void draw() override;
		void update() override;
};

#endif