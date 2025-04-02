#pragma once
#ifndef PIECE_H
#define PIECE_H

#include <vector>
#include <utility>


#include "include/raylib-cpp.hpp"
#include "config.h"
#include "isometric.h"
#include "RenderQueue.h"

class Board;

using namespace std;

class Piece {
    protected:
        raylib::Texture2D* atlas; // Pointer to the texture atlas
        raylib::Rectangle spriteRect;
        raylib::Rectangle frozenSpriteRect;

        int player; // Player identifier (1 or 2)
        int moves = 0;
        float opacity = 1.0f;

		raylib::Vector3 offset = { 0.0f, 0.0f, 0.0f }; // For animation purposes

        int frozen = 0;

        string name;

    public:
        Piece(raylib::Texture2D* texture, int player, string name);
        virtual ~Piece();

        void draw(RenderQueue& renderQueue, float x, float y, float z, bool hidden = false); // Base draw method

        void drawIcon(int x, int y);

		void setOffset(raylib::Vector3 offset);

        /// <summary>
        /// Update called every frame
        /// </summary>
        void update();

        /// <summary>
        /// Updates the piece, called after every move
        /// </summary>
        void updateState();
        
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

        /// <summary>
        /// Determines if a move is legal
        /// </summary>
        /// <param name="x">The x position of this piece</param>
        /// <param name="y">The y position of this piece</param>
        /// <param name="board">A reference to the board this piece is on</param>
        /// <param name="moveX">The destination X of the move to check</param>
        /// <param name="moveY">The destination Y of the move to check</param>
        /// <returns>true if the move is legal, false if not</returns>
        bool isLegalMove(int x, int y, Board& board, int moveX, int moveY);

        /// <summary>
        /// Increments the move count by one
        /// </summary>
        void move();

        Color getColor() const;

        /// <summary>
        /// Gets the player that owns this piece
        /// </summary>
        /// <returns>The number of the player that owns this piece</returns>
        int getPlayer();

        /// <summary>
        /// Sets the piece to frozen for a certain number of turns
        /// </summary>
        /// <param name="frozenTurns">The number of turns to freeze the piece for</param>
        void setFrozen(int frozenTurns);

        /// <summary>
        /// Determines whether the piece is immobile
        /// </summary>
        /// <returns>true if the piece is immobile, false if not</returns>
        bool getImmobile();

        bool isSelectable();

        string getName();
};

class Pawn : public Piece
{
    public:
        Pawn(raylib::Texture2D* texture, int player);
        vector<pair<int, int>> getValidMoves(int x, int y, Board& board) override;
};

class Knight : public Piece
{
    public:
        Knight(raylib::Texture2D* texture, int player);
        vector<pair<int, int>> getValidMoves(int x, int y, Board& board) override;
};

class Bishop : public Piece
{
    public:
        Bishop(raylib::Texture2D* texture, int player);
        vector<pair<int, int>> getValidMoves(int x, int y, Board& board) override;
};

class Rook : public Piece
{
    public:
        Rook(raylib::Texture2D* texture, int player);
        vector<pair<int, int>> getValidMoves(int x, int y, Board& board) override;
};

class Queen : public Piece
{
    public:
        Queen(raylib::Texture2D* texture, int player);
        vector<pair<int, int>> getValidMoves(int x, int y, Board& board) override;
};

class King : public Piece
{
    public:
        King(raylib::Texture2D* texture, int player);
        vector<pair<int, int>> getValidMoves(int x, int y, Board& board) override;
};

#endif
