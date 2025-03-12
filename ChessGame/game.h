#pragma once

#include "player.h"
#include "board.h"
#include "event.h"

class Game {
	private:
		vector<Player> players;
		Board board;
		vector<Event*> activeEvents;

		int currentTurn = 0;

		int gameEnd = false;

	public:
		Game(Texture2D* texture);

		void update();

		bool getGameEnd();

		bool playerIsInCheck(int player);

		Player& getPlayer(int player);

		Board& getBoard();

		int getPlayerTurn();

		void movePiece(int pieceRow, int pieceCol, int destinationRow, int destinationCol);
};

