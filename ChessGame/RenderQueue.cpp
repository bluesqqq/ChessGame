#include "RenderQueue.h"
#include <algorithm>
#include "isometric.h"
#include <iostream>

SpriteObject::SpriteObject(raylib::Vector3 position, raylib::Texture2D* atlas, raylib::Rectangle source, Color color, float opacity) : position(position), atlas(atlas), source(source), color(color), opacity(opacity) {}

float SpriteObject::getDepth() {
	return position.x + position.y + position.z;
}


RenderQueue::RenderQueue() {
	queue = std::vector<SpriteObject>();
}

void RenderQueue::draw() {
	for (SpriteObject& spriteObject : queue) {
		raylib::Vector2 screenPosition = IsoToScreen(spriteObject.position + raylib::Vector3(0.0, 0.0, (spriteObject.source.height / TILE_HEIGHT) - 1.0));

		DrawTextureRec(*spriteObject.atlas, spriteObject.source, screenPosition, Fade(spriteObject.color, spriteObject.opacity));
	}
}

void RenderQueue::addSpriteObject(SpriteObject spriteObject) {
	queue.push_back(spriteObject);
}

void RenderQueue::sortQueue() {
	std::sort(queue.begin(), queue.end(), [](SpriteObject a, SpriteObject b) {
		return a.getDepth() < b.getDepth();
	});
}

void RenderQueue::clearQueue() {
	queue.clear();
}

int RenderQueue::numberOfSpriteObjects() {
	return queue.size();
}
