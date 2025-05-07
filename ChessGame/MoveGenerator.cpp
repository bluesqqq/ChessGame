#include "MoveGenerator.h"

MoveGenerator::MoveGenerator(Game& game) {
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
					board[rank][file] = { type, player, hasMoved };
				}
				else {
					board[rank][file] = { PieceType::NO_PIECE, -1, false };
				}
			}
			else {
				board[rank][file] = { PieceType::NO_PIECE, -1, false };
			}
		}
	}

	if (gameBoard.hasEnPassantableCell()) {
		enPassantableCell = gameBoard.getEnPassantableCell();
	}

	currentPlayer = game.getPlayerTurn();

	printBoard();
}

MoveGenerator::MoveGenerator(string fen) {
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
		}
		else if (c == '/') { // Slash = next rank
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

int MoveGenerator::evaluateBoard() {
	int evaluation = 0;

	for (int rank = 0; rank < 8; rank++) {
		for (int file = 0; file < 8; file++) {
			PieceRepr& tile = board[rank][file];

			int pieceScore = getPieceValue(tile.type);
			float positionalStrength = getPostionalStrength(Cell(rank, file), tile.type, tile.player);

			pieceScore += 100 * positionalStrength; // Positional strength can max score to 150%

			if (tile.player == currentPlayer) {
				evaluation += pieceScore;
			}
			else {
				evaluation -= pieceScore;
			}
		}
	}

	return evaluation + rand() % 50;
}

void MoveGenerator::printBoard() {
	cout << endl << "BOARD REPRESENTATION: " << endl;

	for (int rank = 7; rank >= 0; rank--) {
		for (int file = 0; file < 8; file++) {
			cout << " " << getPieceString(board[rank][file].type, board[rank][file].player);
		}
		cout << endl;
	}
	cout << endl;
}

bool MoveGenerator::isInsideBoard(int rank, int file) const { return rank >= 0 && rank <= 7 && file >= 0 && file <= 7; }

bool MoveGenerator::isInsideBoard(Cell cell) const { return isInsideBoard(cell.rank, cell.file); }

bool MoveGenerator::isEmpty(int rank, int file) const { return board[rank][file].type == PieceType::NO_PIECE; }

bool MoveGenerator::isEmpty(Cell cell) const { return isEmpty(cell.rank, cell.file); }

bool MoveGenerator::isEnemyKing(Cell cell, int player) {
	PieceRepr piece = getPiece(cell);
	return piece.type == PieceType::KING && piece.player != player;
}

bool MoveGenerator::isOpponent(int rank, int file, int player) const { return board[rank][file].player != player && board[rank][file].player != -1; }

bool MoveGenerator::isOpponent(Cell cell, int player) const { return isOpponent(cell.rank, cell.file, player); }


void MoveGenerator::addSlidingMoves(const Cell& start, const vector<Cell>& directions, int player, vector<Move>& moves, bool attacksOnly) {
	for (const Cell& dir : directions) {
		Cell newCell = start + dir;
		while (isInsideBoard(newCell)) {
			if (isEmpty(newCell)) {
				moves.push_back(Move(start, newCell));
			}
			else {
				if (attacksOnly || isOpponent(newCell, player)) {
					moves.push_back(Move(start, newCell));
				}
				if (!(attacksOnly && isEnemyKing(newCell, player))) break; // Stop if the path is blocked (unless were getting the attacks and this is enemy king)
			}
			newCell += dir;
		}
	}
}

void MoveGenerator::addOffsetMoves(const Cell& start, const vector<Cell>& offsets, int player, vector<Move>& moves, bool attacksOnly) {
	for (const Cell& offset : offsets) {
		Cell newCell = start + offset;
		if (attacksOnly || (isInsideBoard(newCell) && (isEmpty(newCell) || isOpponent(newCell, player)))) {
			moves.push_back(Move(start, newCell));
		}
	}
}

Cell MoveGenerator::findKing(int player) {
	for (int rank = 0; rank < 8; ++rank) {
		for (int file = 0; file < 8; ++file) {
			if (board[rank][file].type == PieceType::KING && board[rank][file].player == player) {
				return Cell(rank, file);
			}
		}
	}
	return Cell(-1, -1); // King not found
}

vector<Move> MoveGenerator::getMovesForPiece(Cell cell, bool attacksOnly) {
	PieceRepr tile = getPiece(cell);

	PieceType type = tile.type;

	if (type == PieceType::NO_PIECE) return {};

	vector<Move> moves;

	switch (type) {
	case PieceType::PAWN: {
		int dir = (tile.player == 1) ? 1 : -1; // player 1 goes up, player 2 down

		// Forward move
		Cell nextCell = cell + Cell(dir, 0);

		if (!attacksOnly && isInsideBoard(nextCell) && isEmpty(nextCell)) {
			moves.push_back(Move(cell, nextCell));

			// Double move from starting rank
			if (!tile.hasMoved) {
				nextCell += Cell(dir, 0);
				if (isInsideBoard(nextCell) && isEmpty(nextCell)) {
					moves.push_back(Move(cell, nextCell, true, MoveFlag::EN_PASSANTABLE));
				}
			}
		}

		// Diagonal captures
		for (int dFile = -1; dFile <= 1; dFile += 2) {
			Cell attackCell = cell + Cell(dir, dFile);
			if (attacksOnly || (isInsideBoard(attackCell) && isOpponent(attackCell, tile.player))) {
				moves.push_back(Move(cell, attackCell));
			}
		}

		// En passant
		if (enPassantableCell.has_value()) {
			Cell enPassantCell = enPassantableCell.value();
			if (enPassantCell == cell + Cell(0, -1)) { // En passant left
				moves.push_back(Move(cell, cell + Cell(dir, -1), true, MoveFlag::EN_PASSANT));
			}
			else if (enPassantCell == cell + Cell(0, 1)) { // En passant right
				moves.push_back(Move(cell, cell + Cell(dir, 1), true, MoveFlag::EN_PASSANT));
			}
		}

		break;
	}

	case PieceType::KNIGHT: {
		vector<Cell> offsets = { { -2,-1 }, { -2,1 }, { -1,-2 }, { -1,2 }, { 1,-2 }, { 1,2 }, { 2,-1 }, { 2,1 } };
		addOffsetMoves(cell, offsets, tile.player, moves, attacksOnly);
		break;
	}

	case PieceType::BISHOP: {
		vector<Cell> directions = { {1,1}, {1,-1}, {-1,1}, {-1,-1} };
		addSlidingMoves(cell, directions, tile.player, moves, attacksOnly);
		break;
	}

	case PieceType::ROOK: {
		vector<Cell> directions = { {0,1}, {0,-1}, {-1,0}, {1,0} };
		addSlidingMoves(cell, directions, tile.player, moves, attacksOnly);
		break;
	}

	case PieceType::QUEEN: {
		vector<Cell> directions = { {1,0}, {-1,0}, {0,1}, {0,-1}, {1,1}, {1,-1}, {-1,1}, {-1,-1} };
		addSlidingMoves(cell, directions, tile.player, moves, attacksOnly);
		break;
	}

	case PieceType::KING: {
		vector<Cell> offsets = { {1,0}, {-1,0}, {0,1}, {0,-1}, {1,1}, {1,-1}, {-1,1}, {-1,-1} };

		addOffsetMoves(cell, offsets, tile.player, moves, attacksOnly);

		// Castling
		if (!tile.hasMoved) {
			PieceRepr leftRook = getPiece(Cell(cell.rank, 0));
			PieceRepr rightRook = getPiece(Cell(cell.rank, 7));

			if (leftRook.type == PieceType::ROOK && leftRook.hasMoved == false) {
				bool blocked = false;
				// Check if the squares between the king and rook are empty
				for (int i = cell.file - 1; i > 0; i--) {
					if (!isEmpty(cell.rank, i)) {
						blocked = true;
						break;
					}
				}

				if (!blocked) moves.push_back(Move(cell, cell + Cell(0, -2), false, MoveFlag::CASTLE));
			}

			if (rightRook.type == PieceType::ROOK && rightRook.hasMoved == false) {
				bool blocked = false;
				// Check if the squares between the king and rook are empty
				for (int i = cell.file + 1; i < 7; i++) {
					if (!isEmpty(cell.rank, i)) {
						blocked = true;
						break;
					}
				}

				if (!blocked) moves.push_back(Move(cell, cell + Cell(0, 2), false, MoveFlag::CASTLE));
			}
		}

		break;
	}
	}

	return moves;
}

vector<Move> MoveGenerator::getAllMoves(int player, bool attacksOnly) {
	vector<Move> moves;

	for (int rank = 0; rank < 8; ++rank) {
		for (int file = 0; file < 8; ++file) {
			Cell cell = Cell(rank, file);

			PieceRepr tile = getPiece(cell);

			if (tile.player == player) {
				vector<Move> pieceMoves = getMovesForPiece(cell, attacksOnly);

				for (auto& move : pieceMoves) moves.push_back(move);
			}
		}
	}

	return moves;
}

vector<Move> MoveGenerator::getAllLegalMoves(int player) {
	// Get all of the players moves
	vector<Move> pseudoLegalMoves = getAllMoves(player);
	vector<Move> legalMoves;

	Cell kingCell = findKing(player); // Get the player's king's cell

	// Get the cells that are pinned and the cells that can block checks
	PinAndCheckBlockCell cellData = getPinnedAndCheckBlockingCells(player);

	// Get the cells that the opponent can attack
	vector<Move> opponentAttackingMoves = getAllMoves((player % 2) + 1, true);

	vector<Cell> opponentAttackingCells;

	// Count how many pieces are attacking the king
	int checkingPieces = 0;

	for (const Move& attackingMove : opponentAttackingMoves) {
		if (attackingMove.to == kingCell) {
			checkingPieces++;
			cellData.checkBlockCells.push_back(attackingMove.from); // Add the piece that is attacking the king to the check block cells
		}
		opponentAttackingCells.push_back(attackingMove.to);
	}

	bool hasPinnedCells = cellData.hasPinnedCells();
	bool hasCheckBlockCells = cellData.hasCheckBlockCells();

	if (!hasPinnedCells && !hasCheckBlockCells) return pseudoLegalMoves;

	for (Move& move : pseudoLegalMoves) {
		Cell from = move.from;
		Cell to = move.to;
		bool isKingMove = (from == kingCell); // This move is the king moving

		if (checkingPieces > 1) { // Double check forces king to move to an unattacked cell

			if (!isKingMove) continue; // Only allow king moves

			// Only allow the king to move to cells not attacked by the opponent
			if (find(opponentAttackingCells.begin(), opponentAttackingCells.end(), to) == opponentAttackingCells.end()) {
				legalMoves.push_back(move);
			}

		}
		else if (checkingPieces == 1) { // Single check, either king can move, or a piece can block if possible

			// Only allow the king to move to cells not attacked by the opponent
			if (isKingMove) {
				if (find(opponentAttackingCells.begin(), opponentAttackingCells.end(), to) == opponentAttackingCells.end()) {
					legalMoves.push_back(move);
				}
			}
			else { // Not a king move, so it can only be a piece move that blocks the check
				if (find(cellData.checkBlockCells.begin(), cellData.checkBlockCells.end(), to) != cellData.checkBlockCells.end()) {
					if (find(cellData.pinnedCells.begin(), cellData.pinnedCells.end(), from) != cellData.pinnedCells.end()) { // Piece we're trying to move is pinned
						if (find(cellData.pinnedCells.begin(), cellData.pinnedCells.end(), to) == cellData.pinnedCells.end()) {
							continue; // Skip this move, it puts the king in check
						}
					}

					legalMoves.push_back(move);
				}
			}

		}
		else {

			if (find(cellData.pinnedCells.begin(), cellData.pinnedCells.end(), from) != cellData.pinnedCells.end()) { // Piece we're trying to move is pinned
				if (find(cellData.pinnedCells.begin(), cellData.pinnedCells.end(), to) == cellData.pinnedCells.end()) {
					continue; // Skip this move, it puts the king in check
				}
			}

			legalMoves.push_back(move);
		}
	}

	return legalMoves;
}

PinAndCheckBlockCell MoveGenerator::getPinnedAndCheckBlockingCells(int player) {
	PinAndCheckBlockCell result;

	Cell kingCell = findKing(player);

	if (kingCell == Cell(-1, -1)) return {}; // King not found

	const vector<Cell> directions = { {1,0}, {-1,0}, {0,1}, {0,-1}, {1,1}, {1,-1}, {-1,1}, {-1,-1} };

	for (auto& dir : directions) {
		vector<Cell> ray; // Similar to raycasing

		Cell newCell = kingCell;

		bool foundFriendly = false;

		while (newCell.isInBounds()) {
			newCell += dir;
			PieceRepr piece = getPiece(newCell);

			if (piece.player == player) {
				if (foundFriendly) break; // Two friendly pieces = not pinned

				foundFriendly = true; // One friendly piece = pinned
			}
			else if (piece.player != -1) { // Enemy!
				bool isDiagonal = abs(dir.rank) == abs(dir.file);
				bool isStraight = dir.rank == 0 || dir.file == 0;

				bool canPin = piece.type == PieceType::QUEEN || (isDiagonal && piece.type == PieceType::BISHOP) || (isStraight && piece.type == PieceType::ROOK);

				if (!canPin) break;

				ray.push_back(newCell);

				if (foundFriendly) {
					result.pinnedCells.insert(result.pinnedCells.end(), ray.begin(), ray.end());
				}
				else {
					result.checkBlockCells.insert(result.checkBlockCells.end(), ray.begin(), ray.end());
				}

				break;
			}

			ray.push_back(newCell); // Continue ray until we collide or go out of bounds
		}
	}

	if (!result.pinnedCells.empty()) result.pinnedCells.push_back(kingCell); // Add the king to the list of pinned pieces
	if (!result.checkBlockCells.empty()) result.checkBlockCells.push_back(kingCell); // Add the king to the list of pinned pieces

	return result;
}

void MoveGenerator::makeMove(Move move) {
	MoveMemory moveMemory = { move, getPiece(move.from), getPiece(move.to), enPassantableCell };

	moveHistory.push(moveMemory); // Add the move history to the stack

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
			}
			else { // Right castle
				// Move the rook
				Cell rightRookCell = Cell(move.from.rank, 7);
				Cell rightRookTargetCell = Cell(move.to) + Cell(0, -1);

				movePiece(rightRookCell, rightRookTargetCell);
			}
			break;
		}
		}
	}
	else {
		enPassantableCell = nullopt;
	}

	currentPlayer = (currentPlayer % 2) + 1; // Switch players
}

void MoveGenerator::undoMove() {
	MoveMemory moveMemory = moveHistory.top();
	moveHistory.pop();

	Move move = moveMemory.move;
	PieceRepr previousFrom = moveMemory.from;
	PieceRepr previousTo = moveMemory.to;
	optional<Cell> previousEnPassant = moveMemory.enPassantableCell;

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

	currentPlayer = (currentPlayer % 2) + 1; // Switch players
}

void MoveGenerator::movePiece(Cell from, Cell to, bool moved) {
	setPiece(to, getPiece(from));
	getPiece(to).hasMoved = moved; // Set the piece (at the new destination cell) to moved
	removePiece(from);
}

void MoveGenerator::removePiece(Cell cell) { board[cell.rank][cell.file] = { PieceType::NO_PIECE, -1, false }; }

void MoveGenerator::setPiece(Cell cell, PieceRepr tile) { board[cell.rank][cell.file] = tile; }

PieceRepr& MoveGenerator::getPiece(Cell cell) { return board[cell.rank][cell.file]; }

void MoveGenerator::sortMoves(vector<Move>& moves) {
	for (Move& move : moves) {
		int moveScoreGuess = 0;
		PieceType movePieceType = getPiece(move.from).type;
		PieceType capturePieceType = getPiece(move.to).type;

		if (capturePieceType != PieceType::NO_PIECE) {
			moveScoreGuess = 10 * getPieceValue(capturePieceType) - getPieceValue(movePieceType);
		}

		if (move.flag == MoveFlag::PROMOTION) {
			// TODO: change this when i let the AI choose which promotion
			moveScoreGuess += getPieceValue(PieceType::QUEEN);
		}
	}
}

int MoveGenerator::search(int depth, int alpha, int beta) {
	if (depth == 0) return evaluateBoard();

	vector<Move> moves = getAllMoves(currentPlayer);
	if (moves.empty()) {
		return INT_MIN; // Nothing is worse than losing
	}

	sortMoves(moves);

	int bestScore = INT_MIN;

	for (Move& move : moves) {
		makeMove(move);
		int score = -search(depth - 1, -beta, -alpha); // Move thats good for opponent is bad for us
		undoMove();

		bestScore = max(bestScore, score);

		alpha = max(alpha, score);

		if (alpha >= beta) {
			break; // Beta cut-off
		}

	}

	return bestScore;
}

Move MoveGenerator::chooseMove(int ply) {
	cout << "Searching moves for player #" << currentPlayer << "..." << endl;
	vector<Move> allMoves = getAllLegalMoves(currentPlayer);

	sortMoves(allMoves);

	Move bestMove;
	int bestScore = INT_MIN;

	cout << "Number of possible moves: " << allMoves.size() << endl;
	cout << "Checking follow-up moves (with a depth of " << ply << " plies)..." << endl;

	double startTime = GetTime();

	// Top level: check each move separately
	for (auto& move : allMoves) {
		makeMove(move);
		int score = -search(ply - 1, INT_MIN + 1, INT_MAX - 1);
		undoMove();

		if (score > bestScore) {
			bestScore = score;
			bestMove = move;
		}
	}

	double elapsedTime = GetTime() - startTime;

	cout << " Done! Took: " << elapsedTime << " seconds" << endl;

	cout << "Move chosen: " << bestMove.from.getAlgebraicNotation() << " to " << bestMove.to.getAlgebraicNotation() << endl;

	return bestMove;
}