#pragma once
#ifndef PIECE_H
#define PIECE_H

#include <vector>
#include <utility>


#include "include/raylib-cpp.hpp"
#include "config.h"
#include "isometric.h"

class Board;

using namespace std;

class Piece
{
    protected:
        Texture2D* atlas; // Pointer to the texture atlas
        Rectangle source;

        int player; // Player identifier (1 or 2)
        int moves = 0;
        float opacity = 1.0f;

        string name;

    public:
        Piece(Texture2D* texture, int player, string name, Rectangle source);
        virtual ~Piece();

        void draw(int x, int y, float z, bool hidden = false); // Base draw method

        void drawIcon(int x, int y);

        
        virtual vector<pair<int, int>> getValidMoves(int x, int y, Board& board);
        vector<pair<int, int>> getLegalMoves(int x, int y, Board& board);

        bool isValidMove(int x, int y, Board& board, int moveX, int moveY);
        bool isLegalMove(int x, int y, Board& board, int moveX, int moveY);

        void move();

        Color getColor() const;
        int getPlayer();
};

class Pawn : public Piece
{
    private:

    public:
        Pawn(Texture2D* texture, int player);
        vector<pair<int, int>> getValidMoves(int x, int y, Board& board) override;
};

class Knight : public Piece
{
    public:
        Knight(Texture2D* texture, int player);
        vector<pair<int, int>> getValidMoves(int x, int y, Board& board) override;
};

class Bishop : public Piece
{
public:
    Bishop(Texture2D* texture, int player);
    vector<pair<int, int>> getValidMoves(int x, int y, Board& board) override;
};

class Rook : public Piece
{
    public:
        Rook(Texture2D* texture, int player);
        vector<pair<int, int>> getValidMoves(int x, int y, Board& board) override;
};

class Queen : public Piece
{
    public:
        Queen(Texture2D* texture, int player);
        vector<pair<int, int>> getValidMoves(int x, int y, Board& board) override;
};

class King : public Piece
{
    public:
        King(Texture2D* texture, int player);
        vector<pair<int, int>> getValidMoves(int x, int y, Board& board) override;
};

#endif
