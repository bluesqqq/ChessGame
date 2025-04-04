#include "Theme.h"

Theme::Theme(Background* background, TileType defaultWhite, TileType defaultBlack) : background(background), defaultWhite(defaultWhite), defaultBlack(defaultBlack) {}

void Theme::drawBackground() {
	if (background) {
		background->draw();
	} else {
		ClearBackground(WHITE);
	}
}

void Theme::updateBackground() {
	if (background) {
		background->update();
	}
}

TileType Theme::getDefaultWhite() const {
	return defaultWhite;
}

TileType Theme::getDefaultBlack() const {
	return defaultBlack;
}