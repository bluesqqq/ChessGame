#ifndef CUSTOMTILES_H
#define CUSTOMTILES_H

#include "tile.h"
#include "RenderQueue.h"
#include "Theme.h"
#include "include/raylib-cpp.hpp"

class BasicTile : public Tile {
    private:
        raylib::Texture2D* atlas;

    public:
        BasicTile(raylib::Texture2D* texture);

        void draw(Theme& theme, RenderQueue& renderQueue, int x, int y, float z, bool selected, bool hide) override;
};

class IceTile : public Tile {
    private:
        raylib::Texture2D* atlas;

    public:
        IceTile(raylib::Texture2D* texture);

        void draw(Theme& theme, RenderQueue& renderQueue, int x, int y, float z, bool selected, bool hide) override;

        void applyTileEffect(Board& board) override;
};

class BreakingTile : public Tile {
    private:
        raylib::Texture2D* atlas;

    public:
        BreakingTile(raylib::Texture2D* texture);

        void draw(Theme& theme, RenderQueue& renderQueue, int x, int y, float z, bool selected, bool hide) override;

        void applyTileEffect(Board& board) override;
};

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class ConveyorTile : public Tile {
    private:
        raylib::Texture2D* atlas;
        Direction direction;

    public:
        ConveyorTile(raylib::Texture2D* texture, Direction direction);

        void draw(Theme& theme, RenderQueue& renderQueue, int x, int y, float z, bool selected, bool hide) override;

        void applyTileEffect(Board& board) override;
};

class PortalTile : public Tile {
    private:
        raylib::Texture2D* atlas;
        int portalNumber;

    public:
        PortalTile(raylib::Texture2D* texture, int portalNumber);

        void draw(Theme& theme, RenderQueue& renderQueue, int x, int y, float z, bool selected, bool hide) override;

        void applyTileEffect(Board& board) override;
};

#endif