#pragma once

#include "player.h"
#include "board.h"

class Game
{
	private:
		vector<Player> players;
		Board board;

		int currentTurn = 0;

	public:
		Game(Texture2D* texture);

		Player& getPlayer(int player);

		Board& getBoard();

		int getPlayerTurn();

		void movePiece(int pieceRow, int pieceCol, int destinationRow, int destinationCol);

};

