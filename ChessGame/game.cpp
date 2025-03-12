#include "game.h"
#include <iostream>

Game::Game(Texture2D* texture) : board(texture, players)
{
	players.push_back(Player("Player 1"));
	players.push_back(Player("Player 2"));
}

void Game::update() {
	// Check if the game ended
	int currentPlayer = getPlayerTurn();
	if (board.isInCheckmate(currentPlayer) || board.isInStalemate(currentPlayer)) {
		gameEnd = true;
		return;
	}

	board.update();

	if (currentTurn == 5) activeEvents.push_back(new GravityEvent());

	for (auto& event : activeEvents) {
		event->updateEvent(board);
	}

	activeEvents.erase(
		remove_if(activeEvents.begin(), activeEvents.end(), [](Event* event) {
			return event->getLifetime() <= 0;
			}),
		activeEvents.end()
	);
}

bool Game::getGameEnd() {
	return gameEnd;
}

bool Game::playerIsInCheck(int player) {
	return board.isInCheck(player);
}

Player& Game::getPlayer(int player) {
	if (player <= 0 || player > players.size()) {
		throw std::runtime_error("Player index out of bounds");
	}
	return players[player - 1];
}

Board& Game::getBoard() {
	return board;
}

int Game::getPlayerTurn() {
	return (currentTurn % 2) + 1;
}

void Game::movePiece(int pieceRow, int pieceCol, int destinationRow, int destinationCol) {
	if (board.isLegalMove(getPlayerTurn(), pieceRow, pieceCol, destinationRow, destinationCol)) {

		Piece* discardedPiece = board.movePiece(getPlayerTurn(), pieceRow, pieceCol, destinationRow, destinationCol);

		// Add the discarded piece to that player's list of discarded pieces
		if (discardedPiece) {
			players[discardedPiece->getPlayer() - 1].addDiscardedPiece(discardedPiece);
		}

		currentTurn++;
		update();
	}
}