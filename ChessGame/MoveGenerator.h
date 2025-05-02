#pragma once

#include "game.h"
#include "Move.h"
#include <functional>
#include <iostream>
#include <cctype>
#include "Cell.h"
#include "Personality.h"
#include <string>

struct TileRepresentation {
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

struct CellMove {
	Cell from;
	Cell to;
	CellMove() : from(-1, -1), to(-1, -1) {}
	CellMove(Cell to, Cell from) : from(from), to(to) {}
	CellMove(int fromRank, int fromFile, int toRank, int toFile) : from(fromRank, fromFile), to(toRank, toFile) {}
};

class MoveGenerator {
	private:
		TileRepresentation board[8][8];
		Personality personality;
		float positionalStrengthMultipliers[8][8] = {
			1.1, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.1,
			1.3, 1.5, 1.7, 1.7, 1.7, 1.7, 1.5, 1.3,
			1.3, 1.7, 1.9, 1.9, 1.9, 1.9, 1.7, 1.3,
			1.3, 1.7, 1.9, 2.0, 2.0, 1.9, 1.7, 1.3,
			1.3, 1.7, 1.9, 2.0, 2.0, 1.9, 1.7, 1.3,
			1.3, 1.7, 1.9, 1.9, 1.9, 1.9, 1.7, 1.3,
			1.3, 1.5, 1.7, 1.7, 1.7, 1.7, 1.5, 1.3,
			1.1, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.1
		};

	public:
		MoveGenerator(Game& game) {
			Personality personality = Personality{ 50, 50, 50, 50 };

			Board& gameBoard = game.getBoard();

			for (int rank = 0; rank < 8; rank++) {
				for (int file = 0; file < 8; file++) {

					Tile* tile = gameBoard.getTile(Cell(rank, file));

					if (tile) {
						Piece* piece = tile->getPiece();

						if (piece) {
							PieceType type = piece->getType();
							int8_t player = piece->getPlayer();
							bool hasMoved = piece->getNumberOfMoves() > 0;
							board[rank][file] = { type, player, hasMoved};
						} else {
							board[rank][file] = { PieceType::NO_PIECE, -1, false};
						}
					} else {
						board[rank][file] = { PieceType::NO_PIECE, -1, false};
					}
				}
			}

			printBoard();
		}

		MoveGenerator(string fen) {

			cout << "Constructing board representation from FEN: " << fen << endl;

			int rank = 7;
			int file = 0;

			for (char c : fen) {
				if (isdigit(c)) { // Number = skip that many files
					int spaces = c - '0';
					while (spaces > 0) {
						board[rank][file] = { PieceType::NO_PIECE, -1, false };
						file++;
						spaces--;
					}
				} else if (c == '/') { // Slash = next rank
					rank--;
					file = 0;
				}
				else {
					PieceType type = PieceType::NO_PIECE;
					int8_t player = -1;
					switch (c) {
						case 'p': type = PieceType::PAWN;   player = 2; break;
						case 'r': type = PieceType::ROOK;   player = 2; break;
						case 'n': type = PieceType::KNIGHT; player = 2; break;
						case 'b': type = PieceType::BISHOP; player = 2; break;
						case 'q': type = PieceType::QUEEN;  player = 2; break;
						case 'k': type = PieceType::KING;   player = 2; break;
						case 'P': type = PieceType::PAWN;   player = 1; break;
						case 'R': type = PieceType::ROOK;   player = 1; break;
						case 'N': type = PieceType::KNIGHT; player = 1; break;
						case 'B': type = PieceType::BISHOP; player = 1; break;
						case 'Q': type = PieceType::QUEEN;  player = 1; break;
						case 'K': type = PieceType::KING;   player = 1; break;
					}
					board[rank][file] = { type, player, false };
					file++;
				}
			}

			// Set moved for any piece out of place
			for (int rank = 0; rank < 8; rank++) {
				for (int file = 0; file < 8; file++) {
					switch (board[rank][file].type) {
						case PieceType::PAWN: {
							if ((board[rank][file].player == 1 && rank != 1) || (board[rank][file].player == 2 && rank != 6))
								board[rank][file].hasMoved = true;
							break;
						}
						case PieceType::ROOK: {
							if ((board[rank][file].player == 1 && (rank != 0 && file != 0) && (rank != 0 && file != 7)) ||
								(board[rank][file].player == 2 && (rank != 7 && file != 0) && (rank != 7 && file != 7)))
								board[rank][file].hasMoved = true;
							break;
						}
						case PieceType::KNIGHT: {
							if ((board[rank][file].player == 1 && (rank != 0 && file != 1) && (rank != 0 && file != 6)) ||
								(board[rank][file].player == 2 && (rank != 7 && file != 1) && (rank != 7 && file != 6)))
								board[rank][file].hasMoved = false;
							break;
						}
						case PieceType::BISHOP: {
							if ((board[rank][file].player == 1 && (rank != 0 && file != 2) && (rank != 0 && file != 5)) ||
								(board[rank][file].player == 2 && (rank != 7 && file != 2) && (rank != 7 && file != 5)))
								board[rank][file].hasMoved = false;
							break;
						}
						case PieceType::QUEEN: {
							if ((board[rank][file].player == 1 && rank != 0 && file != 3) ||
								(board[rank][file].player == 2 && rank != 7 && file != 3))
								board[rank][file].hasMoved = false;
							break;
						}
						case PieceType::KING: {
							if ((board[rank][file].player == 1 && rank != 0 && file != 4) ||
								(board[rank][file].player == 2 && rank != 7 && file != 4))
								board[rank][file].hasMoved = false;
							break;
						}
					}
				}
			}

			printBoard();
		}

		/// <summary>
		/// Evaluates a "score" for a board from one player's persepctive
		/// </summary>
		/// <param name="player">Player to consider</param>
		/// <returns>A score where higher is better for the specificed player</returns>
		int evaluateBoard(int player) {
			int score = 0;
			int pieces = 0;

			for (int rank = 0; rank < 8; rank++) {
				for (int file = 0; file < 8; file++) {
					TileRepresentation& tile = board[rank][file];

					if (tile.type != PieceType::NO_PIECE) {
						int value = getPieceValue(tile.type) * 1; //positionalStrengthMultipliers[rank][file];

						score += (tile.player == player) ? value : -value;
					}
				}
			}

			return score;
		}

		/// <summary>
		/// Gets the piece value for a specific piece type
		/// </summary>
		/// <param name="type">The piece to get the value of</param>
		/// <returns>The value of the piece</returns>
		int getPieceValue(PieceType type) {
			switch (type) {
				case PieceType::PAWN: return 100;
				case PieceType::KNIGHT: return 320;
				case PieceType::BISHOP: return 330;
				case PieceType::ROOK: return 500;
				case PieceType::QUEEN: return 900;
				case PieceType::KING: return 20000;
				default: return 0;
			}
		}

		void printBoard() {
			std::cout << std::endl << " BOARD REPRESENTATION: " << std::endl;

			for (int rank = 7; rank >= 0; rank--) {
				for (int file = 0; file < 8; file++) {
					std::cout << getPieceString(board[rank][file].type, board[rank][file].player) << " ";
				}
				std::cout << std::endl;
			}
		}

		string getPieceString(PieceType type, int8_t player = 1) {
			string pieceString = "-";
			switch (type) {
				case PieceType::PAWN: pieceString = "P"; break;
				case PieceType::KNIGHT: pieceString = "N"; break;
				case PieceType::BISHOP: pieceString = "B"; break;
				case PieceType::ROOK: pieceString = "R"; break;
				case PieceType::QUEEN: pieceString = "Q"; break;
				case PieceType::KING: pieceString = "K"; break;
			}

			if (player != 1) {
				pieceString = tolower(pieceString[0]);
			}

			return pieceString;
		}

		bool isInsideBoard(int rank, int file) { return rank >= 0 && rank <= 7 && file >= 0 && file <= 7; }

		bool isEmpty(int rank, int file) { return board[rank][file].type == PieceType::NO_PIECE; }

		bool isOpponent(int rank, int file, int player) { return board[rank][file].player != player && board[rank][file].player != -1; }

		bool isKingInCheck(int player) {
			int kingRank = -1, kingFile = -1;

			// Find the player's king
			for (int rank = 0; rank < 8; rank++) {
				for (int file = 0; file < 8; file++) {
					if (board[rank][file].type == PieceType::KING && board[rank][file].player == player) {
						kingRank = rank;
						kingFile = file;
						break;
					}
				}
			}

			if (kingRank == -1 || kingFile == -1) {
				return true; // King not found = ????????
			}

			// Check if any opponent piece can capture the king
			for (int rank = 0; rank < 8; rank++) {
				for (int file = 0; file < 8; file++) {
					if (isOpponent(rank, file, player)) {
						vector<CellMove> pieceMoves = getMovesForPiece(rank, file);

						Cell kingCell = Cell(kingRank, kingFile);

						for (const CellMove& move : pieceMoves) {
							if (move.to == kingCell) return true; // King is under attack
						}
					}
				}
			}

			return false; // King is safe
		}

		std::vector<CellMove> getMovesForPiece(int rank, int file) {
			TileRepresentation tile = board[rank][file];

			PieceType type = tile.type;

			if (type == PieceType::NO_PIECE) return {};

			std::vector<CellMove> moves;

			switch (type) {
				case PieceType::PAWN: {
					int dir = (tile.player == 1) ? 1 : -1; // player 1 goes up, player 2 down

					// Forward move
					int nextRank = rank + dir;
					if (isInsideBoard(nextRank, file) && isEmpty(nextRank, file)) {
						moves.push_back({ rank, file, nextRank, file });

						// Double move from starting rank
						if (!tile.hasMoved) {
							int jumpRank = rank + 2 * dir;
							if (isInsideBoard(jumpRank, file) && isEmpty(jumpRank, file)) {
								moves.push_back({ rank, file, jumpRank, file });
							}
						}
					}

					// Diagonal captures
					for (int dFile = -1; dFile <= 1; dFile += 2) {
						int attackFile = file + dFile;
						if (isInsideBoard(nextRank, attackFile) && isOpponent(nextRank, attackFile, tile.player)) {
							moves.push_back({ rank, file, nextRank, attackFile });
						}
					}

					break;
				}

				case PieceType::KNIGHT: {
					int offsets[8][2] = {
						{ -2, -1 }, { -2, 1 },
						{ -1, -2 }, { -1, 2 },
						{ 1, -2 }, { 1, 2 },
						{ 2, -1 }, { 2, 1 }
					};

					for (auto& offset : offsets) {
						int newRank = rank + offset[0];
						int newFile = file + offset[1];
						if (isInsideBoard(newRank, newFile) &&
							(isEmpty(newRank, newFile) || isOpponent(newRank, newFile, tile.player))) {
							moves.push_back({ rank, file, newRank, newFile });
						}
					}
					break;
				}

				case PieceType::BISHOP: {
					int directions[4][2] = { {1,1}, {1,-1}, {-1,1}, {-1,-1} };

					for (auto& dir : directions) {
						int newRank = rank + dir[0], newFile = file + dir[1];
						while (isInsideBoard(newRank, newFile)) {
							if (isEmpty(newRank, newFile)) {
								moves.push_back({ rank, file, newRank, newFile });
							}
							else {
								if (isOpponent(newRank, newFile, tile.player)) {
									moves.push_back({ rank, file, newRank, newFile });
								}
								break;
							}
							newRank += dir[0];
							newFile += dir[1];
						}
					}
					break;
				}

				case PieceType::ROOK: {
					int directions[4][2] = { {0,1}, {0,-1}, {-1,0}, {1,0} };

					for (auto& dir : directions) {
						int newRank = rank + dir[0], newFile = file + dir[1];
						while (isInsideBoard(newRank, newFile)) {
							if (isEmpty(newRank, newFile)) {
								moves.push_back({ rank, file, newRank, newFile });
							}
							else {
								if (isOpponent(newRank, newFile, tile.player)) {
									moves.push_back({ rank, file, newRank, newFile });
								}
								break;
							}
							newRank += dir[0];
							newFile += dir[1];
						}
					}
					break;
				}

				case PieceType::QUEEN: {
					int directions[8][2] = {
						{1,0}, {-1,0}, {0,1}, {0,-1},
						{1,1}, {1,-1}, {-1,1}, {-1,-1}
					};

					for (auto& dir : directions) {
						int newRank = rank + dir[0], newFile = file + dir[1];
						while (isInsideBoard(newRank, newFile)) {
							if (isEmpty(newRank, newFile)) {
								moves.push_back({ rank, file, newRank, newFile });
							}
							else {
								if (isOpponent(newRank, newFile, tile.player)) {
									moves.push_back({ rank, file, newRank, newFile });
								}
								break;
							}
							newRank += dir[0];
							newFile += dir[1];
						}
					}
					break;
				}

				case PieceType::KING: {
					int directions[8][2] = {
						{1,0}, {-1,0}, {0,1}, {0,-1},
						{1,1}, {1,-1}, {-1,1}, {-1,-1}
					};

					for (auto& dir : directions) {
						int newRank = rank + dir[0], newFile = file + dir[1];
						if (isInsideBoard(newRank, newFile) &&
							(isEmpty(newRank, newFile) || isOpponent(newRank, newFile, tile.player))) {
							moves.push_back({ rank, file, newRank, newFile });
						}
					}
					break;
				}
			}

			return moves;
		}

		/// <summary>
		/// Generates all move for a player on the current board
		/// </summary>
		/// <param name="player">The player to generate moves for</param>
		/// <returns>A vector of all moves for the player specified</returns>
		std::vector<CellMove> getAllMoves(int player) {
			std::vector<CellMove> moves;

			for (int rank = 0; rank < 8; ++rank) {
				for (int file = 0; file < 8; ++file) {
					TileRepresentation tile = board[rank][file];

					if (tile.player == player) {
						vector<CellMove> pieceMoves = getMovesForPiece(rank, file);

						for (auto& move : pieceMoves) moves.push_back(move);
					}
				}
			}

			return moves;
		}

		std::vector<CellMove> getAllLegalMoves(int player) {
			std::vector<CellMove> pseudoLegalMoves = getAllMoves(player);
			std::vector<CellMove> legalMoves;

			for (CellMove& move : pseudoLegalMoves) {
				// Save current state
				TileRepresentation previousFrom = board[move.from.rank][move.from.file];
				TileRepresentation previousTo = board[move.to.rank][move.to.file];

				makeMove(move);

				// Check if making the move leaves the player's king in check
				if (!isKingInCheck(player)) {
					legalMoves.push_back(move);
				}

				undoMove(move, previousFrom, previousTo);
			}

			return legalMoves;
		}

		// Function to make a move
		void makeMove(CellMove move) {
			TileRepresentation& fromTile = board[move.from.rank][move.from.file];
			TileRepresentation& toTile = board[move.to.rank][move.to.file];

			// Move the piece
			toTile = fromTile;
			fromTile = { PieceType::NO_PIECE, -1, false };

			// Mark the piece as moved if necessary
			if (toTile.type != PieceType::NO_PIECE) {
				toTile.hasMoved = true;
			}
		}

		// Function to undo a move
		void undoMove(CellMove move, TileRepresentation previousFrom, TileRepresentation previousTo) {
			board[move.from.rank][move.from.file] = previousFrom;
			board[move.to.rank][move.to.file] = previousTo;
		}

		CellMove chooseMove(int player, int ply) {
			int movesCalculated = 0;

			std::cout << "Attempting to find best move for player #" << player << std::endl;
			std::vector<CellMove> allMoves = getAllLegalMoves(player);
			CellMove bestMove;
			int bestScore = INT_MIN;

			std::cout << "Number of found moves: " << allMoves.size() << std::endl;
			std::cout << "Checking moves: ";

			// Pure minimax (no alpha-beta)
			std::function<int(int, int)> minimax = [&](int currentPlayer, int depth) {
				if (depth == 0) {
					return evaluateBoard(player); // Always evaluate from original player's perspective
				}

				int bestMoveValue = (currentPlayer == player) ? INT_MIN : INT_MAX;
				std::vector<CellMove> moves = getAllLegalMoves(currentPlayer);

				for (auto& move : moves) {
					// Save current state
					TileRepresentation previousFrom = board[move.from.rank][move.from.file];
					TileRepresentation previousTo = board[move.to.rank][move.to.file];

					// Make move
					makeMove(move);

					// Recurse
					int score = minimax((currentPlayer) % 2 + 1, depth - 1);
					movesCalculated++;

					// Undo move
					undoMove(move, previousFrom, previousTo);

					// Maximizing or minimizing
					if (currentPlayer == player) {
						bestMoveValue = std::max(bestMoveValue, score);
					}
					else {
						bestMoveValue = std::min(bestMoveValue, score);
					}
				}

				return bestMoveValue;
				};

			// Top level: check each move separately
			for (auto& move : allMoves) {
				cout << "#";
				TileRepresentation previousFrom = board[move.from.rank][move.from.file];
				TileRepresentation previousTo = board[move.to.rank][move.to.file];

				makeMove(move);

				int score = minimax((player + 1) % 2, ply - 1);
				movesCalculated++;

				undoMove(move, previousFrom, previousTo);

				if (score > bestScore) {
					bestScore = score;
					bestMove = move;
				}
			}

			cout << " Done!" << endl;

			printShannonNumber(movesCalculated, ply);

			return bestMove;
		}

		void printShannonNumber(int calculatedMoves, int plies) { // https://en.wikipedia.org/wiki/Shannon_number
			std::vector<long long> shannonNumbers = { 20, 400, 8902, 197281, 4865609, 119060324, 2863350967, 69586103104, 1669531250000 };

			std::cout << "Total moves calculated: " << calculatedMoves << std::endl;

			int expectedMoves = 0;
				
			for (int i = 0; i < plies; i++) {
				expectedMoves += shannonNumbers[i];
			}

			std::cout << "Expected moves calculated: " << expectedMoves << std::endl;
		}
};

