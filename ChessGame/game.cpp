#include "game.h"

Game::Game(Texture2D* texture) : board(texture, players)
{
	players.push_back(Player("Player 1"));
	players.push_back(Player("Player 2"));
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
	if (board.movePiece(getPlayerTurn(), pieceRow, pieceCol, destinationRow, destinationCol)) {
		currentTurn++;
	}
}