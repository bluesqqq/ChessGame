#pragma once

#include "game.h"
#include "Move.h"
#include <functional>
#include <iostream>
#include "Cell.h"

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

struct TileMove {
	int fromRow, fromCol;
	int toRow, toCol;
};

class MoveGenerator {
	private:
		TileRepresentation board[8][8];

	public:
		MoveGenerator(Game& game) {
			Board& gameBoard = game.getBoard();

			std::cout << std::endl << " BOARD REPRESENTATION: " << std::endl;

			for (int rank = 0; rank < 8; rank++) {
				for (int file = 0; file < 8; file++) {

					Tile* tile = gameBoard.getTile(Cell(rank + 1, file + 1));

					if (tile) {
						Piece* piece = tile->getPiece();

						if (piece) {
							board[rank][file] = { piece->getType(), (int8_t)(piece->getPlayer() - 1), piece->getNumberOfMoves() > 1};
						} else {
							board[rank][file] = { PieceType::NO_PIECE, -1, false};
						}
					} else {
						board[rank][file] = { PieceType::NO_PIECE, -1, false};
					}
				}
				std::cout << std::endl;
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

			for (int row = 0; row < 8; row++) {
				for (int col = 0; col < 8; col++) {
					TileRepresentation& tile = board[row][col];

					if (tile.type != PieceType::NO_PIECE) {
						int value = getPieceValue(tile.type);
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
			for (int row = 0; row < 8; row++) {
				for (int col = 0; col < 8; col++) {
					std::cout << " " << getPieceString(board[row][col].type) << " ";
				}
				std::cout << std::endl;
			}
		}

		string getPieceString(PieceType type) {
			switch (type) {
			case PieceType::PAWN: return "P";
			case PieceType::KNIGHT: return "N";
			case PieceType::BISHOP: return "B";
			case PieceType::ROOK: return "R";
			case PieceType::QUEEN: return "Q";
			case PieceType::KING: return "K";
			default: return "-";
			}
		}

		bool isInsideBoard(int row, int col) { return row >= 0 && row < 8 && col >= 0 && col < 8; }

		bool isEmpty(int row, int col) { return board[row][col].type == PieceType::NO_PIECE; }

		bool isOpponent(int row, int col, int player) { return board[row][col].player != player && board[row][col].player != -1; }

		/// <summary>
		/// Generates all move for a player on the current board
		/// </summary>
		/// <param name="player">The player to generate moves for</param>
		/// <returns>A vector of all moves for the player specified</returns>
		std::vector<TileMove> generateMoves(int player) {
			std::vector<TileMove> moves;

			for (int row = 0; row < 8; ++row) {
				for (int col = 0; col < 8; ++col) {
					TileRepresentation tile = board[row][col];

					if (tile.player == player) {
						PieceType type = tile.type;

						// Add logic for each piece type
						switch (type) {
							case PieceType::PAWN: {
								int dir = (player == 0) ? 1 : -1; // player 0 goes up, player 1 down

								// Forward move
								int nextRow = row + dir;
								if (isInsideBoard(nextRow, col) && isEmpty(nextRow, col)) {
									moves.push_back({ row, col, nextRow, col });

									// Double move from starting row
									if (!tile.hasMoved) {
										int jumpRow = row + 2 * dir;
										if (isInsideBoard(jumpRow, col) && isEmpty(jumpRow, col)) {
											moves.push_back({ row, col, jumpRow, col });
										}
									}
								}

								// Diagonal captures
								for (int dcol = -1; dcol <= 1; dcol += 2) {
									int attackCol = col + dcol;
									if (isInsideBoard(nextRow, attackCol) && isOpponent(nextRow, attackCol, player)) {
										moves.push_back({ row, col, nextRow, attackCol });
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
									int newRow = row + offset[0];
									int newCol = col + offset[1];
									if (isInsideBoard(newRow, newCol) &&
										(isEmpty(newRow, newCol) || isOpponent(newRow, newCol, player))) {
										moves.push_back({ row, col, newRow, newCol });
									}
								}
								break;
							}

							case PieceType::BISHOP: {
								int directions[4][2] = { {1,1}, {1,-1}, {-1,1}, {-1,-1} };

								for (auto& dir : directions) {
									int r = row + dir[0], c = col + dir[1];
									while (isInsideBoard(r, c)) {
										if (isEmpty(r, c)) {
											moves.push_back({ row, col, r, c });
										}
										else {
											if (isOpponent(r, c, player)) {
												moves.push_back({ row, col, r, c });
											}
											break;
										}
										r += dir[0];
										c += dir[1];
									}
								}
								break;
							}

							case PieceType::ROOK: {
								int directions[4][2] = { {0,1}, {0,-1}, {-1,0}, {1,0} };

								for (auto& dir : directions) {
									int r = row + dir[0], c = col + dir[1];
									while (isInsideBoard(r, c)) {
										if (isEmpty(r, c)) {
											moves.push_back({ row, col, r, c });
										}
										else {
											if (isOpponent(r, c, player)) {
												moves.push_back({ row, col, r, c });
											}
											break;
										}
										r += dir[0];
										c += dir[1];
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
									int r = row + dir[0], c = col + dir[1];
									while (isInsideBoard(r, c)) {
										if (isEmpty(r, c)) {
											moves.push_back({ row, col, r, c });
										}
										else {
											if (isOpponent(r, c, player)) {
												moves.push_back({ row, col, r, c });
											}
											break;
										}
										r += dir[0];
										c += dir[1];
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
									int r = row + dir[0], c = col + dir[1];
									if (isInsideBoard(r, c) &&
										(isEmpty(r, c) || isOpponent(r, c, player))) {
										moves.push_back({ row, col, r, c });
									}
								}
								break;
							}
						}
					}
				}
			}

			return moves;
		}

		// Function to make a move
		void makeMove(TileMove move) {
			TileRepresentation& fromTile = board[move.fromRow][move.fromCol];
			TileRepresentation& toTile = board[move.toRow][move.toCol];

			// Move the piece
			toTile = fromTile;
			fromTile = { PieceType::NO_PIECE, -1, false };

			// Mark the piece as moved if necessary
			if (toTile.type != PieceType::NO_PIECE) {
				toTile.hasMoved = true;
			}
		}

		// Function to undo a move
		void undoMove(TileMove move, TileRepresentation previousFrom, TileRepresentation previousTo) {
			board[move.fromRow][move.fromCol] = previousFrom;
			board[move.toRow][move.toCol] = previousTo;
		}

		TileMove chooseMove(int player, int ply) {
			int movesCalculated = 0;

			player -= 1;  // Adjust player number (assumes player is 1 or 2 input)
			std::cout << "Attempting to find best move for player #" << player << std::endl;
			std::vector<TileMove> allMoves = generateMoves(player);
			TileMove bestMove;
			int bestScore = INT_MIN;

			std::cout << "Number of found moves: " << allMoves.size() << std::endl;
			std::cout << "Checking moves... " << std::endl;

			// minimax function only returns scores now
			std::function<int(int, int, int, int)> minimax = [&](int currentPlayer, int depth, int alpha, int beta) {
				if (depth == 0) {
					return evaluateBoard(player); // Always evaluate from the original player's perspective
				}

				int bestMoveValue = (currentPlayer == player) ? INT_MIN : INT_MAX;
				std::vector<TileMove> moves = generateMoves(currentPlayer);

				for (auto& move : moves) {
					// Save current state
					TileRepresentation previousFrom = board[move.fromRow][move.fromCol];
					TileRepresentation previousTo = board[move.toRow][move.toCol];

					// Make move
					makeMove(move);

					// Recurse
					int score = minimax((currentPlayer + 1) % 2, depth - 1, alpha, beta);
					movesCalculated++;

					// Undo move
					undoMove(move, previousFrom, previousTo);

					// Maximizing or minimizing
					if (currentPlayer == player) {
						bestMoveValue = std::max(bestMoveValue, score);
						alpha = std::max(alpha, bestMoveValue);
					}
					else {
						bestMoveValue = std::min(bestMoveValue, score);
						beta = std::min(beta, bestMoveValue);
					}

					// Alpha-beta pruning
					if (beta <= alpha) {
						break;
					}
				}

				return bestMoveValue;
				};

			// Top level: check each move separately
			for (auto& move : allMoves) {
				TileRepresentation previousFrom = board[move.fromRow][move.fromCol];
				TileRepresentation previousTo = board[move.toRow][move.toCol];

				makeMove(move);

				int score = minimax((player + 1) % 2, ply - 1, INT_MIN, INT_MAX);
				movesCalculated++;

				undoMove(move, previousFrom, previousTo);

				if (score > bestScore) {
					bestScore = score;
					bestMove = move;
				}
			}

			std::cout << "Total moves Calculated: " << movesCalculated << std::endl;
			std::cout << "Decided to move "
				<< getPieceString(board[bestMove.fromRow][bestMove.fromCol].type)
				<< " from " << bestMove.fromRow + 1 << "," << bestMove.fromCol + 1
				<< " to " << bestMove.toRow + 1 << "," << bestMove.toCol + 1
				<< std::endl;

			return bestMove;
		}

};

