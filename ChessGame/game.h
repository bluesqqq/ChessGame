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

		int updateWaitFrames = 60;
		bool queuedForUpdate = false;

		void update();

		void updateMusicStreams();

		/// <summary>
		/// Determines if the game has ended
		/// </summary>
		/// <returns>true if the game has ended, false if not</returns>
		bool getGameEnd();

		/// <summary>
		/// Determines if a player is in check
		/// </summary>
		/// <param name="player">The player to check for check</param>
		/// <returns>true if the player is in check, false if not</returns>
		bool playerIsInCheck(int player);

		/// <summary>
		/// Gets a player with their number
		/// </summary>
		/// <param name="player">The number of the player to get</param>
		/// <returns>A reference to the player</returns>
		Player& getPlayer(int player);

		/// <summary>
		/// Gets the board in play
		/// </summary>
		/// <returns>A reference to the board in play</returns>
		Board& getBoard();

		/// <summary>
		/// Gets which players turn it is
		/// </summary>
		/// <returns>The number of the player who is currently making their move</returns>
		int getPlayerTurn();

		void movePiece(int pieceRow, int pieceCol, int destinationRow, int destinationCol);
};

