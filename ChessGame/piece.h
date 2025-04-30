#ifndef PIECE_H
#define PIECE_H

#include <vector>
#include <utility>

#include "include/raylib-cpp.hpp"
#include "config.h"
#include "isometric.h"
#include "RenderQueue.h"
#include "Cell.h"
#include "animation.h"
#include <optional>

class Board;

enum class PieceType {
    NO_PIECE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING 
};

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

        PieceType pieceType;

        optional<Animation> animation;

    public:
        Piece(raylib::Texture2D* texture, int player, PieceType pieceName);
        virtual ~Piece();

        /************************************|
                 GAME LOOP FUNCTIONS
        |************************************/

        void draw(RenderQueue& renderQueue, float x, float y, float z, bool hidden = false); // Base draw method

        void drawIcon(int x, int y);

        /// <summary>
        /// Update called every frame
        /// </summary>
        void update();

        /// <summary>
        /// Updates the piece, called after every move
        /// </summary>
        void updateState();

        /************************************|
                  ANIMATION FUNCTIONS
        |************************************/

        /// <summary>
        /// Determines if the piece currently has an animation
        /// </summary>
        /// <returns>True if there is an animation, false if not</returns>
        bool hasAnimation();

        /// <summary>
        /// Sets the piece's animation and plays it
        /// </summary>
        /// <param name="anim">The animation to play</param>
        void playAnimation(Animation anim);

        /// <summary>
        /// Determines if the animation is finished
        /// </summary>
        /// <returns>Tre if the animation is finished or there is no animation, false if not</returns>
        bool animationFinished();

        /// <summary>
        /// Removes the animation from the piece
        /// </summary>
        void removeAnimation();

		/// <summary>
		/// Sets the draw offset of the piece
		/// </summary>
		/// <param name="offset">The offset position as a Vector3</param>
		void setOffset(raylib::Vector3 offset);
        
        /************************************|
                    MOVE FUNCTIONS
        |************************************/

        /// <summary>
        /// Increments the move count by one
        /// </summary>
        void move();

        /// <summary>
        /// Get the number of moves this piece has made, including movement from tiles
        /// </summary>
        /// <returns>The number of moves this piece has made</returns>
        int getNumberOfMoves();

        /// <summary>
        /// Returns a list of valid moves based on this piece's ruleset. Note that this function does not account for
        /// any of the games rules like king-checking, and all the other various rules that might be implemented.
        /// </summary>
        /// <param name="board">The board this piece is on</param>
        /// <returns>A vector of all cells this piece can move to</returns>
        virtual std::vector<Cell> getValidMoves(Board& board) = 0;

        /// <summary>
        /// Returns a list of legal moves based on this piece's ruleset and the ruleset of the game.
        /// </summary>
        /// <param name="board">The board this piece is on</param>
        /// <returns>>A vector of all cells this piece can legally move to</returns>
        std::vector<Cell> getLegalMoves(Board& board);
        
        /// <summary>
        /// Determines if a move is legal
        /// </summary>
        /// <param name="board">The board this piece is on</param>
        /// <param name="move">The cell of the desired move</param>
        /// <returns>True if the move is legal, false if not</returns>
        bool isLegalMove(Board& board, Cell move);

        /************************************|
                   STATE FUNCTIONS
        |************************************/

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
        /// Determines if the piece is immobile
        /// </summary>
        /// <returns>true if the piece is immobile, false if not</returns>
        bool getImmobile();

        /// <summary>
        /// Determines if the piece is selectable
        /// </summary>
        /// <returns>true if the piece is selectable, false if not</returns>
        bool isSelectable();

        /// <summary>
        /// Gets the name of the piece
        /// </summary>
        /// <returns>The name of the piece as a string</returns>
        std::string getName();

        PieceType getType();
};

class Pawn : public Piece {
    public:
        Pawn(raylib::Texture2D* texture, int player);
        std::vector<Cell> getValidMoves(Board& board) override;
};

class Knight : public Piece {
    public:
        Knight(raylib::Texture2D* texture, int player);
        std::vector<Cell> getValidMoves(Board& board) override;
};

class Bishop : public Piece {
    public:
        Bishop(raylib::Texture2D* texture, int player);
        std::vector<Cell> getValidMoves(Board& board) override;
};

class Rook : public Piece {
    public:
        Rook(raylib::Texture2D* texture, int player);
        std::vector<Cell> getValidMoves(Board& board) override;
};

class Queen : public Piece {
    public:
        Queen(raylib::Texture2D* texture, int player);
        std::vector<Cell> getValidMoves(Board& board) override;
};

class King : public Piece {
    public:
        King(raylib::Texture2D* texture, int player);
        std::vector<Cell> getValidMoves(Board& board) override;
};

#endif
