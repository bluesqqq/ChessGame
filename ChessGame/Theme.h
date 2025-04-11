#ifndef THEME_H
#define THEME_H

#include "Background.h"
#include "textures.h"

class Theme {
	private:
		Background* background;

		TileType defaultWhite;
		TileType defaultBlack;

	public:
		Theme(Background* background, TileType defaultWhite, TileType defaultBlack);

		void drawBackground();

		void updateBackground();

		TileType getDefaultWhite() const;
		TileType getDefaultBlack() const;
};

#endif