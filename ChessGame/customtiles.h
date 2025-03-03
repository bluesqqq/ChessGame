#pragma once
#include "tile.h"

class BasicTile : public Tile
{
    private:
        Texture2D* atlas;

    public:
        BasicTile(Texture2D* texture);

        void draw(int x, int y, float z, bool selected, bool hide) override;

        void update() override;
        bool isSelectable() override;
};

class IceTile : public Tile
{
    private:
        Texture2D* atlas;

    public:
        IceTile(Texture2D* texture);

        void draw(int x, int y, float z, bool selected, bool hide) override;

        void update() override;
        bool isSelectable() override;
};

class BreakingTile : public Tile
{
    private:
        Texture2D* atlas;

    public:
        BreakingTile(Texture2D* texture);

        void draw(int x, int y, float z, bool selected, bool hide) override;

        void update() override;
        bool isSelectable() override;
};

