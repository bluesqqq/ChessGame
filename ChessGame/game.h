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

		raylib::Music musicNormal;
		raylib::Music musicIce;
		raylib::Music musicBreak;
		raylib::Music musicConveyor;

	public:
		Game(Texture2D* texture);

		void update();

		void updateMusicStreams();

		bool getGameEnd();

		bool playerIsInCheck(int player);

		Player& getPlayer(int player);

		Board& getBoard();

		int getPlayerTurn();

		void movePiece(int pieceRow, int pieceCol, int destinationRow, int destinationCol);
};

