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
        Rectangle spriteRect;
        Rectangle frozenSpriteRect;

        int player; // Player identifier (1 or 2)
        int moves = 0;
        float opacity = 1.0f;

        int frozen = 0;

        string name;

    public:
        Piece(Texture2D* texture, int player, string name);
        virtual ~Piece();

        void draw(float x, float y, float z, bool hidden = false); // Base draw method

        void drawIcon(int x, int y);

        /// <summary>
        /// Updates the piece, called after every move
        /// </summary>
        void update();
        
        /// <summary>
        /// Returns a list of valid moves based on this piece's ruleset. Note that this function does not account for
        /// any of the games rules like king-checking, and all the other various rules that might be implemented.
        /// </summary>
        /// <param name="x">X position on the board of this piece</param>
        /// <param name="y">Y position on the board of this piece</param>
        /// <param name="board">Reference to the board object</param>
        /// <returns>A vector of {x, y} pairs representing all possible positions that this piece can move to</returns>
        virtual vector<pair<int, int>> getValidMoves(int x, int y, Board& board);

        /// <summary>
        /// Returns a list of legal moves based on this piece's ruleset and the ruleset of the game.
        /// </summary>
        /// <param name="x">X position on the board of this piece</param>
        /// <param name="y">Y position on the board of this piece</param>
        /// <param name="board">Reference to the board object</param>
        /// <returns>A vector of {x, y} pairs representing all possible positions that this piece can move to</returns>
        vector<pair<int, int>> getLegalMoves(int x, int y, Board& board);

        bool isLegalMove(int x, int y, Board& board, int moveX, int moveY);

        void move();

        Color getColor() const;
        int getPlayer();

        void setFrozen(int frozenTurns);
        bool getImmobile();
};

class Pawn : public Piece
{
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
