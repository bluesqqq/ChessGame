#pragma once

#include "game.h"
#include "Move.h"
#include <functional>
#include <iostream>
#include <cctype>
#include "Cell.h"
#include "Personality.h"
#include <string>
#include <optional>
#include <stack>

using namespace std;

struct PieceRepr {
	/// <summary>
	/// The piece that is on this spot
	/// </summary>
	PieceType type;

	/// <summary>
	/// The player who's piece is on this spot, -1 if no piece
	/// </summary>
	int8_t player;

	bool hasMoved;
};

struct MoveMemory {
	Move move; 
	PieceRepr from;
	PieceRepr to;
	optional<Cell> enPassantableCell;
};

struct PinAndCheckBlockCell {
	vector<Cell> pinnedCells;
	vector<Cell> checkBlockCells;

	bool hasPinnedCells() { return !pinnedCells.empty(); }

	bool hasCheckBlockCells() { return !checkBlockCells.empty(); }
};

class MoveGenerator {
	private:
		PieceRepr board[8][8];
		Personality personality;
		float knightPositionalStrength[8][8] = {
			0.0, 0.2, 0.4, 0.4, 0.4, 0.4, 0.2, 0.0,
			0.2, 0.5, 0.6, 0.6, 0.6, 0.6, 0.5, 0.2,
			0.4, 0.6, 0.8, 0.9, 0.9, 0.8, 0.6, 0.4,
			0.4, 0.6, 0.9, 1.0, 1.0, 0.9, 0.6, 0.4,
			0.4, 0.6, 0.9, 1.0, 1.0, 0.9, 0.6, 0.4,
			0.4, 0.6, 0.8, 0.9, 0.9, 0.8, 0.6, 0.4,
			0.2, 0.5, 0.6, 0.6, 0.6, 0.6, 0.5, 0.2,
			0.0, 0.2, 0.4, 0.4, 0.4, 0.4, 0.2, 0.0
		};

		float kingPositionalStrength[8][8] = {
			0.2, 0.1, 0.1, 0.0, 0.0, 0.1, 0.1, 0.2,
			0.2, 0.1, 0.1, 0.0, 0.0, 0.1, 0.1, 0.2,
			0.4, 0.2, 0.2, 0.0, 0.0, 0.2, 0.2, 0.4,
			0.4, 0.2, 0.2, 0.0, 0.0, 0.2, 0.2, 0.4,
			0.5, 0.4, 0.4, 0.2, 0.2, 0.4, 0.4, 0.5,
			0.7, 0.6, 0.5, 0.4, 0.4, 0.5, 0.6, 0.7,
			0.8, 0.8, 0.6, 0.6, 0.6, 0.6, 0.8, 0.8,
			0.9, 1.0, 0.8, 0.7, 0.7, 0.8, 1.0, 0.9
		};

		float queenPositionalStrength[8][8] = {
			0.0, 0.3, 0.3, 0.4, 0.4, 0.3, 0.3, 0.0,
			0.3, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.3,
			0.3, 0.8, 1.0, 1.0, 1.0, 1.0, 0.8, 0.3,
			0.4, 0.8, 1.0, 1.0, 1.0, 1.0, 0.8, 0.4,
			0.5, 0.8, 1.0, 1.0, 1.0, 1.0, 0.8, 0.4,
			0.3, 1.0, 1.0, 1.0, 1.0, 1.0, 0.8, 0.3,
			0.3, 0.8, 0.9, 0.9, 0.9, 0.8, 0.8, 0.3,
			0.0, 0.3, 0.3, 0.4, 0.4, 0.3, 0.3, 0.0
		};

		float bishopPositionalStrength[8][8] = {
			0.0, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.0,
			0.2, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.2,
			0.2, 0.5, 0.6, 0.8, 0.8, 0.6, 0.5, 0.2,
			0.2, 0.6, 0.6, 0.8, 0.8, 0.6, 0.6, 0.2,
			0.2, 0.5, 1.0, 0.8, 0.8, 1.0, 0.5, 0.2,
			0.2, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.2,
			0.2, 0.8, 0.5, 0.5, 0.5, 0.5, 0.8, 0.2,
			0.5, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.5
		};

		float rookPositionalStrength[8][8] = {
			0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
			0.7, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.7,
			0.0, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.0,
			0.0, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.0,
			0.0, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.0,
			0.0, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.0,
			0.0, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.0,
			0.2, 0.2, 0.4, 0.6, 0.6, 0.4, 0.2, 0.2
		};

		float pawnPositionalStrength[8][8] = {
			0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2,
			1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
			0.4, 0.4, 0.6, 0.8, 0.8, 0.6, 0.4, 0.4,
			0.3, 0.3, 0.4, 0.7, 0.7, 0.4, 0.3, 0.3,
			0.2, 0.2, 0.2, 0.6, 0.6, 0.2, 0.2, 0.2,
			0.3, 0.2, 0.1, 0.3, 0.3, 0.1, 0.2, 0.3,
			0.3, 0.5, 0.5, 0.0, 0.0, 0.5, 0.5, 0.3,
			0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2
		};


		optional<Cell> enPassantableCell = nullopt; // The cell of a pawn that can be captured en passant

		int currentPlayer;

		stack<MoveMemory> moveHistory;

		void addSlidingMoves(const Cell& start, const vector<Cell>& directions, int player, vector<Move>& moves, bool attacksOnly = false);

		void addOffsetMoves(const Cell& start, const vector<Cell>& offsets, int player, vector<Move>& moves, bool attacksOnly = false);

	public:
		MoveGenerator(Game& game);

		MoveGenerator(string fen);

		/************************************|
				 BOARD STATE FUNCTIONS
		|************************************/

		/// <summary>
		/// Checks if the given rank and file are inside the board
		/// </summary>
		/// <param name="rank">The rank of the cell to check</param>
		/// <param name="file">The file of the cell to check</param>
		/// <returns>True if the cell is inside the board, false if not</returns>
		bool isInsideBoard(int rank, int file) const;

		/// <summary>
		/// Checks if the given cell is inside the oard
		/// </summary>
		/// <param name="cell">The cell to check</param>
		/// <returns>True if the cell is inside the board, false if not</returns>
		bool isInsideBoard(Cell cell) const;

		/// <summary>
		/// Checks if the given rank and file have no piece
		/// </summary>
		/// <param name="rank">The rank of the cell to check</param>
		/// <param name="file">The file of the cell to check</param>
		/// <returns>True if there is no piece on the cell, false if not</returns>
		bool isEmpty(int rank, int file) const;

		/// <summary>
		/// Checks if the given cell has no piece
		/// </summary>
		/// <param name="cell">The cell to chek</param>
		/// <returns>True if there is no piece on the cell, false if not</returns>
		bool isEmpty(Cell cell) const;

		/// <summary>
		/// Checks if there is an enemy king on the cell
		/// </summary>
		/// <param name="cell">The cell to check</param>
		/// <param name="player">The player that is not the enemy</param>
		/// <returns>True if there is an enemy king on the cell, false if not</returns>
		bool isEnemyKing(Cell cell, int player);

		/// <summary>
		/// Checks if the given rank and file are occupied by an opponent
		/// </summary>
		/// <param name="rank">The rank of the cell to check</param>
		/// <param name="file">The file of the cell to check</param>
		/// <param name="player">The player to check for opponents of</param>
		/// <returns>True if the cell is occupied by an opponent, false if not</returns>
		bool isOpponent(int rank, int file, int player) const;

		/// <summary>
		/// Checks if the given cell is occupied by an opponent
		/// </summary>
		/// <param name="cell">The cell to check</param>
		/// <param name="player">The player to check for opponents of</param>
		/// <returns>True if the cell is occupied by an opponnt, false if not</returns>
		bool isOpponent(Cell cell, int player) const;

		/************************************|
				 MOVE FUNCTIONS
		|************************************/

		/// <summary>
		/// Makes a move on the board
		/// </summary>
		/// <param name="move">The move to make</param>
		void makeMove(Move move);

		/// <summary>
		/// Undoes the last move mad
		/// </summary>
		void undoMove();

		/// <summary>
		/// Moves a piece from one cell to another
		/// </summary>
		/// <param name="from">The cell of the piece to move</param>
		/// <param name="to">The cell to move the piece to</param>
		/// <param name="moved">Whether or not to set the piece to moved or not</param>
		void movePiece(Cell from, Cell to, bool moved = true);

		/// <summary>
		/// Removes a piece from a cell
		/// </summary>
		/// <param name="cell">The cell to remove the piece from</param>
		void removePiece(Cell cell);

		/// <summary>
		/// Sets a cell to a piece
		/// </summary>
		/// <param name="cell">The cell to set</param>
		/// <param name="tile">The tile to set to</param>
		void setPiece(Cell cell, PieceRepr tile);

		PieceRepr& getPiece(Cell cell);

		/************************************|
		      MOVE GENERATION FUNCTIONS
		|************************************/

		/// <summary>
		/// Finds a specified player's king on the board
		/// </summary>
		/// <param name="player">The player whos king to search for</param>
		/// <returns>The Cell of the player's king, or {-1, -1} if not found</returns>
		Cell findKing(int player);

		/// <summary>
		/// Gets a list of moves for a piece on the board
		/// </summary>
		/// <param name="cell">The cell of the piece</param>
		/// <param name="attacksOnly">Return attacking square as well</param>
		/// <returns>A vector of all possible moves for this piece</returns>
		vector<Move> getMovesForPiece(Cell cell, bool attacksOnly = false);

		/// <summary>
		/// Gets a list of all pseudo-legal piece moves for a player 
		/// </summary>
		/// <param name="player">The player to get the moves of</param>
		/// <param name="attacksOnly">Return attacking square as well</param>
		/// <returns>A vector of all pseudo-legal moves for the player</returns>
		vector<Move> getAllMoves(int player, bool attacksOnly = false);

		/// <summary>
		/// Gets a list of all legal piece moves for a player
		/// </summary>
		/// <param name="player">The player to get the moves of</param>
		/// <returns>A vector of all legal moves for the player</returns>
		vector<Move> getAllLegalMoves(int player);

		/// <summary>
		/// Gets the pinned and check-blocking cells for a player
		/// </summary>
		/// <param name="player"></param>
		/// <returns></returns>
		PinAndCheckBlockCell getPinnedAndCheckBlockingCells(int player);

		/************************************|
				   SEARCH FUNCTIONS
		|************************************/

		float getPostionalStrength(Cell cell, PieceType pieceType, int player) {
			int rank = (player != 1) ? cell.rank : 7 - cell.rank;
			int file = cell.file;

			switch (pieceType) {
				case PieceType::PAWN: return pawnPositionalStrength[rank][file];
				case PieceType::KNIGHT: return knightPositionalStrength[rank][file];
				case PieceType::BISHOP: return bishopPositionalStrength[rank][file];
				case PieceType::ROOK: return rookPositionalStrength[rank][file];
				case PieceType::QUEEN: return queenPositionalStrength[rank][file];
				case PieceType::KING: return kingPositionalStrength[rank][file];
				default: return 0;
			}
		}

		/// <summary>
		/// Evaluates the board from the current player's perspective
		/// </summary>
		/// <returns></returns>
		int evaluateBoard();

		/// <summary>
		/// Sorts a list moves so better moves are likely to come first
		/// </summary>
		/// <param name="moves">The list of moves to sort</param>
		void sortMoves(vector<Move>& moves);

		/// <summary>
		/// Evaluates the best possible score from the current board position, up to a specified depth
		/// </summary>
		/// <param name="depth">The number of plies (half-turns) to search ahead</param>
		/// <param name="alpha">The best score that the maximizing playeris guaranteed to achieve (lower bound)</param>
		/// <param name="beta">The best score that the minimizing player is guaranteed to achieev (upper bound)</param>
		/// <returns>The best score achievable from the current position</returns>
		int search(int depth, int alpha, int beta);

		/// <summary>
		/// Searches all possible moves and chooses a move to play
		/// </summary>
		/// <param name="ply">The number of plies (half-turns) to search ahead</param>
		/// <returns>The move to make on the board</returns>
		Move chooseMove(int ply);

		/************************************|
				   DEBUG FUNCTIONS
		|************************************/

		void printShannonNumber(int calculatedMoves, int plies) { // https://en.wikipedia.org/wiki/Shannon_number
			vector<long long> shannonNumbers = { 20, 400, 8902, 197281, 4865609, 119060324, 2863350967, 69586103104, 1669531250000 };

			cout << endl << "Total moves calculated: " << calculatedMoves << endl;

			int expectedMoves = 0;
				
			for (int i = 0; i < plies; i++) {
				expectedMoves += shannonNumbers[i];
			}

			cout << "Expected moves calculated: " << expectedMoves << endl;
		}

		void printBoard();
};

