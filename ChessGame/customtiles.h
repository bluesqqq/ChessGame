#pragma once
#include "tile.h"
#include "RenderQueue.h"

class BasicTile : public Tile {
    private:
        raylib::Texture2D* atlas;

    public:
        BasicTile(raylib::Texture2D* texture);

        void draw(RenderQueue& renderQueue, int x, int y, float z, bool selected, bool hide) override;

        void updateState(Board& board) override;
        bool isSelectable() override;
};

class IceTile : public Tile {
    private:
        raylib::Texture2D* atlas;

    public:
        IceTile(raylib::Texture2D* texture);

        void draw(RenderQueue& renderQueue, int x, int y, float z, bool selected, bool hide) override;

        void updateState(Board& board) override;
        bool isSelectable() override;
};

class BreakingTile : public Tile {
    private:
        raylib::Texture2D* atlas;

    public:
        BreakingTile(raylib::Texture2D* texture);

        void draw(RenderQueue& renderQueue, int x, int y, float z, bool selected, bool hide) override;

        void updateState(Board& board) override;
        bool isSelectable() override;
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

        void draw(RenderQueue& renderQueue, int x, int y, float z, bool selected, bool hide) override;

        void updateState(Board& board) override;
        bool isSelectable() override;
};