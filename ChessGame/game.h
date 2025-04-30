#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "board.h"
#include "event.h"
#include "RenderQueue.h"
#include "Theme.h"
#include "PromotionMenu.h"
#include <optional>

class Game {
	private:
		vector<Player> players;
		Board board;
		vector<Event*> activeEvents;

		int currentTurn = 1;

		int gameEnd = false;

		raylib::Music musicNormal;
		raylib::Music musicIce;
		raylib::Music musicBreak;
		raylib::Music musicConveyor;
		raylib::Music musicPortal;

		RenderQueue renderQueue;

		Theme theme;

		Cell selectedCell = Cell(-1, -1);

		optional<PromotionMenu> promotionMenu = nullopt;

	public:
		Game(raylib::Texture2D* texture);

		int updateWaitFrames = 60;
		bool queuedForUpdate = false;

		void draw(raylib::Texture2D* atlas);

		/// <summary>
		/// Update called every frame
		/// </summary>
		void update();

		void updateState();

		void updateMusicStreams();

		void setSelectedCell(Cell cell);

		bool isPlayable() {
			return board.isPlayable() && !promotionMenu.has_value();
		}

		RenderQueue& getRenderQueue();

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

		Player& getCurrentPlayer();

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
};

#endif