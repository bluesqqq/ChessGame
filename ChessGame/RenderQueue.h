#ifndef RENDERQUEUE_H
#define RENDERQUEUE_H

#include "include/raylib-cpp.hpp"
#include <vector>

using namespace std;

struct SpriteObject {
	raylib::Vector3 position; // Position of the object in 3D space
	raylib::Texture2D* atlas; // Texture Atlasof the object
	raylib::Rectangle source; // Source rectangle of the object in the texture atlas

	Color color;
	float opacity;

	SpriteObject(raylib::Vector3 position, raylib::Texture2D* atlas, raylib::Rectangle source, Color color = WHITE, float opacity = 1.0f);
	float getDepth();
};;

class RenderQueue {
	private:
		vector<SpriteObject> queue;

	public:
		RenderQueue();

		void draw();

		void addSpriteObject(SpriteObject spriteObject);

		void sortQueue();

		void clearQueue();

		int numberOfSpriteObjects();
};

#endif