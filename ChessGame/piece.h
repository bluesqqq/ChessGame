#pragma once
#ifndef PIECE_H
#define PIECE_H

#include "raylib.h"
#include "config.h"
#include "isometric.h"

class Piece
{
protected:
    Texture2D* atlas; // Pointer to the texture atlas
    int player; // Player identifier (1 or 2)

public:
    Piece(Texture2D* texture, int player) : atlas(texture), player(player) {}
    virtual ~Piece() {}
    virtual void draw(int x, int y, float z) {} // Base draw method

    Color getColor() const {
        return (player == 1) ? WHITE : GRAY;
    }
};

class Pawn : public Piece
{
public:
    Pawn(Texture2D* texture, int player) : Piece(texture, player) {}

    void draw(int x, int y, float z) override
    {
        Rectangle source = { 4 * TILE_SIZE, 1 * TILE_SIZE, TILE_SIZE, TILE_SIZE };
        Vector2 position = IsoToScreen(x, y, z + 1);
        DrawTextureRec(*atlas, source, position, getColor());
    }
};

class Knight : public Piece
{
public:
    Knight(Texture2D* texture, int player) : Piece(texture, player) {}

    void draw(int x, int y, float z) override
    {
        Rectangle source = { 4 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, TILE_SIZE };
        Vector2 position = IsoToScreen(x, y, z + 1);
        DrawTextureRec(*atlas, source, position, getColor());
    }
};

class Bishop : public Piece
{
public:
    Bishop(Texture2D* texture, int player) : Piece(texture, player) {}

    void draw(int x, int y, float z) override
    {
        Rectangle source = { 5 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, TILE_SIZE };
        Vector2 position = IsoToScreen(x, y, z + 1);
        DrawTextureRec(*atlas, source, position, getColor());
    }
};

class Rook : public Piece
{
public:
    Rook(Texture2D* texture, int player) : Piece(texture, player) {}

    void draw(int x, int y, float z) override
    {
        Rectangle source = { 5 * TILE_SIZE, 1 * TILE_SIZE, TILE_SIZE, TILE_SIZE };
        Vector2 position = IsoToScreen(x, y, z + 1);
        DrawTextureRec(*atlas, source, position, getColor());
    }
};

class Queen : public Piece
{
public:
    Queen(Texture2D* texture, int player) : Piece(texture, player) {}

    void draw(int x, int y, float z) override
    {
        Rectangle source = { 6 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, TILE_SIZE * 2 };
        Vector2 position = IsoToScreen(x, y, z + 3);
        DrawTextureRec(*atlas, source, position, getColor());
    }
};

class King : public Piece
{
public:
    King(Texture2D* texture, int player) : Piece(texture, player) {}

    void draw(int x, int y, float z) override
    {
        Rectangle source = { 7 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, TILE_SIZE * 2 };
        Vector2 position = IsoToScreen(x, y, z + 3);
        DrawTextureRec(*atlas, source, position, getColor());
    }
};

#endif
