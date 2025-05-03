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

using namespace std;

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

		optional<Cell> enPassantableCell = nullopt; // The cell of a pawn that can be captured en passant

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

			if (gameBoard.hasEnPassantableCell()) {
				enPassantableCell = gameBoard.getEnPassantableCell();
			}

			printBoard();
		}

		MoveGenerator(string fen) {
			// I might use this for unit testing with stockfish later
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
			cout << endl << "BOARD REPRESENTATION: " << endl;

			for (int rank = 7; rank >= 0; rank--) {
				for (int file = 0; file < 8; file++) {
					cout << " " << getPieceString(board[rank][file].type, board[rank][file].player);
				}
				cout << endl;
			}
			cout << endl;
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
						vector<Move> pieceMoves = getMovesForPiece(rank, file);

						Cell kingCell = Cell(kingRank, kingFile);

						for (const Move& move : pieceMoves) {
							if (move.to == kingCell) return true; // King is under attack
						}
					}
				}
			}

			return false; // King is safe
		}

		std::vector<Move> getMovesForPiece(int rank, int file) {
			TileRepresentation tile = board[rank][file];

			PieceType type = tile.type;

			if (type == PieceType::NO_PIECE) return {};

			std::vector<Move> moves;

			switch (type) {
				case PieceType::PAWN: {
					int dir = (tile.player == 1) ? 1 : -1; // player 1 goes up, player 2 down

					// Forward move
					int nextRank = rank + dir;
					if (isInsideBoard(nextRank, file) && isEmpty(nextRank, file)) {
						moves.push_back(Move(Cell(rank, file), Cell(nextRank, file)));

						// Double move from starting rank
						if (!tile.hasMoved) {
							int jumpRank = rank + 2 * dir;
							if (isInsideBoard(jumpRank, file) && isEmpty(jumpRank, file)) {
								moves.push_back(Move(Cell(rank, file), Cell(jumpRank, file), true, MoveFlag::EN_PASSANTABLE));
							}
						}
					}

					// Diagonal captures
					for (int dFile = -1; dFile <= 1; dFile += 2) {
						int attackFile = file + dFile;
						if (isInsideBoard(nextRank, attackFile) && isOpponent(nextRank, attackFile, tile.player)) {
							moves.push_back(Move(Cell(rank, file), Cell(nextRank, attackFile)));
						}
					}

					// En passant
					if (enPassantableCell.has_value()) {
						Cell enPassantCell = enPassantableCell.value();
						if (enPassantCell == Cell(rank, file - 1)) { // En passant left
							moves.push_back(Move(Cell(rank + dir, file - 1), enPassantCell, true, MoveFlag::EN_PASSANT));
						} else if (enPassantCell == Cell(rank, file + 1)) { // En passant left
							moves.push_back(Move(Cell(rank + dir, file + 1), enPassantCell, true, MoveFlag::EN_PASSANT));
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
							moves.push_back(Move(Cell(rank, file), Cell(newRank, newFile)));
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
								moves.push_back(Move(Cell(rank, file), Cell(newRank, newFile)));
							}
							else {
								if (isOpponent(newRank, newFile, tile.player)) {
									moves.push_back(Move(Cell(rank, file), Cell(newRank, newFile)));
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
								moves.push_back(Move(Cell(rank, file), Cell(newRank, newFile)));
							}
							else {
								if (isOpponent(newRank, newFile, tile.player)) {
									moves.push_back(Move(Cell(rank, file), Cell(newRank, newFile)));
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
								moves.push_back(Move(Cell(rank, file), Cell(newRank, newFile)));
							}
							else {
								if (isOpponent(newRank, newFile, tile.player)) {
									moves.push_back(Move(Cell(rank, file), Cell(newRank, newFile)));
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
							moves.push_back(Move(Cell(rank, file), Cell(newRank, newFile)));
						}
					}

					// Castling
					if (!tile.hasMoved) {
						TileRepresentation leftRook = board[rank][0];
						TileRepresentation rightRook = board[rank][7];

						if (leftRook.type == PieceType::ROOK && leftRook.hasMoved == false) {
							bool blocked = false;
							// Check if the squares between the king and rook are empty
							for (int i = file - 1; i > 0; i--) {
								if (!isEmpty(rank, i)) {
									blocked = true;
									break;
								}
							}

							if (!blocked) moves.push_back(Move(Cell(rank, file), Cell(rank, file - 2), false, MoveFlag::CASTLE));
						}

						if (rightRook.type == PieceType::ROOK && rightRook.hasMoved == false) {
							bool blocked = false;
							// Check if the squares between the king and rook are empty
							for (int i = file + 1; i < 7; i++) {
								if (!isEmpty(rank, i)) {
									blocked = true;
									break;
								}
							}

							if (!blocked) moves.push_back(Move(Cell(rank, file), Cell(rank, file + 2), false, MoveFlag::CASTLE));
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
		std::vector<Move> getAllMoves(int player) {
			std::vector<Move> moves;

			for (int rank = 0; rank < 8; ++rank) {
				for (int file = 0; file < 8; ++file) {
					TileRepresentation tile = board[rank][file];

					if (tile.player == player) {
						vector<Move> pieceMoves = getMovesForPiece(rank, file);

						for (auto& move : pieceMoves) moves.push_back(move);
					}
				}
			}

			return moves;
		}

		std::vector<Move> getAllLegalMoves(int player) {
			std::vector<Move> pseudoLegalMoves = getAllMoves(player);
			std::vector<Move> legalMoves;

			for (Move& move : pseudoLegalMoves) {
				// Save current state
				TileRepresentation previousFrom = board[move.from.rank][move.from.file];
				TileRepresentation previousTo = board[move.to.rank][move.to.file];

				optional<Cell> previousEnPassantableCell = enPassantableCell;

				makeMove(move);

				// Check if making the move leaves the player's king in check
				if (!isKingInCheck(player)) {
					legalMoves.push_back(move);
				}

				undoMove(move, previousFrom, previousTo, previousEnPassantableCell);
			}

			return legalMoves;
		}

		// Function to make a move
		void makeMove(Move move) {
			movePiece(move.from, move.to, true);

			// Mark the piece as moved if necessary
			if (getPiece(move.to).type != PieceType::NO_PIECE) {
				getPiece(move.to).hasMoved = true;
			}

			if (move.flag.has_value()) {
				switch (move.flag.value()) {
					case MoveFlag::EN_PASSANTABLE: {
						enPassantableCell = move.to;
						break;
					}
					case MoveFlag::EN_PASSANT: {
						removePiece(enPassantableCell.value());
						break;
					}
					case MoveFlag::CASTLE: {
						int8_t player = getPiece(move.to).player;

						if (move.to.file < move.from.file) { // Left castle
							// Move the rook
							Cell leftRookCell = Cell(move.from.rank, 0);
							Cell leftRookTargetCell = Cell(move.to) + Cell(0, 1);

							movePiece(leftRookCell, leftRookTargetCell);
						} else { // Right castle
							// Move the rook
							Cell rightRookCell = Cell(move.from.rank, 7);
							Cell rightRookTargetCell = Cell(move.to) + Cell(0, -1);

							movePiece(rightRookCell, rightRookTargetCell);
						}
						break;
					}
				}
			} else {
				enPassantableCell = nullopt;
			}
		}

		void movePiece(Cell from, Cell to, bool moved = true) {
			setPiece(to, getPiece(from));
			getPiece(to).hasMoved = moved; // Set the piece (at the new destination cell) to moved
			removePiece(from);
		}

		// Function to undo a move
		void undoMove(Move move, TileRepresentation previousFrom, TileRepresentation previousTo, optional<Cell> previousEnPassant) {
			setPiece(move.from, previousFrom);
			setPiece(move.to, previousTo);

			enPassantableCell = previousEnPassant;

			if (move.flag.has_value()) {
				switch (move.flag.value()) {
					case MoveFlag::EN_PASSANT: {
						// Put the piece captured in en passant back
						setPiece(enPassantableCell.value(), { PieceType::PAWN, (int8_t)(getPiece(move.from).player % 2 + 1), true });
						break;
					}
					case MoveFlag::CASTLE: {
						int8_t player = previousFrom.player;

						if (move.to.file < move.from.file) { // Left castle
							// Move the rook back
							movePiece(move.to + Cell(0, 1), Cell(move.to.rank, 0), false);
						}
						else { // Right castle
							// Move the rook back
							movePiece(move.to + Cell(0, -1), Cell(move.to.rank, 7), false);
						}

						break;
					}
				}
			}
		}

		void removePiece(Cell cell) {
			board[cell.rank][cell.file] = { PieceType::NO_PIECE, -1, false };
		}

		void setPiece(Cell cell, TileRepresentation tile) {
			board[cell.rank][cell.file] = tile;
		}

		TileRepresentation& getPiece(Cell cell) {
			return board[cell.rank][cell.file];
		}

		Move chooseMove(int player, int ply) {
			int movesCalculated = 0;

			std::cout << "Searching moves for player #" << player << "..." << std::endl;
			std::vector<Move> allMoves = getAllLegalMoves(player);
			Move bestMove;
			int bestScore = INT_MIN;

			std::cout << "Number of possible moves: " << allMoves.size() << std::endl;
			std::cout << "Checking follow-up moves (with a depth of " << ply << " plies)..." << endl;

			// Pure minimax (no alpha-beta)
			std::function<int(int, int)> minimax = [&](int currentPlayer, int depth) {
				if (depth == 0) {
					return evaluateBoard(player); // Always evaluate from original player's perspective
				}

				int bestMoveValue = (currentPlayer == player) ? INT_MIN : INT_MAX;
				std::vector<Move> moves = getAllLegalMoves(currentPlayer);

				for (auto& move : moves) {
					// Save current state
					TileRepresentation previousFrom = board[move.from.rank][move.from.file];
					TileRepresentation previousTo = board[move.to.rank][move.to.file];

					optional<Cell> previousEnPassantableCell = enPassantableCell;

					// Make move
					makeMove(move);

					// Recurse
					int score = minimax((currentPlayer) % 2 + 1, depth - 1);
					movesCalculated++;

					// Undo move
					undoMove(move, previousFrom, previousTo, previousEnPassantableCell);

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

			double startTime = GetTime();

			// Top level: check each move separately
			for (auto& move : allMoves) {
				TileRepresentation previousFrom = board[move.from.rank][move.from.file];
				TileRepresentation previousTo = board[move.to.rank][move.to.file];

				optional<Cell> previousEnPassantableCell = enPassantableCell;

				makeMove(move);

				float multiplier = 1;

				if (move.flag.has_value() && move.flag.value() == MoveFlag::EN_PASSANT) {
					multiplier = -1000;
				}

				int score = minimax((player + 1) % 2, ply - 1) * multiplier;
				movesCalculated++;

				undoMove(move, previousFrom, previousTo, previousEnPassantableCell);

				if (score > bestScore) {
					bestScore = score;
					bestMove = move;
				}
			}

			double elapsedTime = GetTime() - startTime;

			cout << " Done! Took: " << elapsedTime << " seconds" << endl;

			printShannonNumber(movesCalculated, ply);

			return bestMove;
		}

		void printShannonNumber(int calculatedMoves, int plies) { // https://en.wikipedia.org/wiki/Shannon_number
			std::vector<long long> shannonNumbers = { 20, 400, 8902, 197281, 4865609, 119060324, 2863350967, 69586103104, 1669531250000 };

			std::cout << endl << "Total moves calculated: " << calculatedMoves << std::endl;

			int expectedMoves = 0;
				
			for (int i = 0; i < plies; i++) {
				expectedMoves += shannonNumbers[i];
			}

			std::cout << "Expected moves calculated: " << expectedMoves << std::endl;
		}
};

