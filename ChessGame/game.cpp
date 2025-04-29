#include "game.h"
#include <iostream>

Game::Game(raylib::Texture2D* texture) : board(texture, players), theme(new SkyBackground(), TILE_GRASS_LIGHT, TILE_GRASS_DARK) {
	players.push_back(Player("Player 1"));
	players.push_back(Player("Player 2"));

	// Load all the music streams
	musicNormal   = LoadMusicStream("resources/theme1_normal.wav");
	musicIce      = LoadMusicStream("resources/theme1_ice.wav");
	musicBreak    = LoadMusicStream("resources/theme1_break.wav");
	musicConveyor = LoadMusicStream("resources/theme1_conveyor.wav");
	musicPortal   = LoadMusicStream("resources/theme1_portal.wav");

	SetMusicVolume(musicNormal, 1.0f);
	SetMusicVolume(musicIce, 0.0f);
	SetMusicVolume(musicBreak, 0.0f);
	SetMusicVolume(musicConveyor, 0.0f);
	SetMusicVolume(musicPortal, 0.0f);

	PlayMusicStream(musicNormal);
	PlayMusicStream(musicIce);
	PlayMusicStream(musicBreak);
	PlayMusicStream(musicConveyor);
	PlayMusicStream(musicPortal);
}

void Game::draw() {
	theme.drawBackground();

	board.draw(theme, renderQueue, getPlayerTurn(), selectedCell);

	renderQueue.sortQueue();
	renderQueue.draw();
	renderQueue.clearQueue();
}

void Game::update() {
	theme.updateBackground();

	// Check if the board is ready for a move and the player has made a move
	if (board.isPlayable()) {
		Player& currentPlayer = getCurrentPlayer();
		if (currentPlayer.hasMove()) {
			Move playerMove = currentPlayer.getMove();
			// Can add extra failsafe handling here if needed, but not for now

			/* Piece* piece = playerMove.from->getPiece();
			if (dynamic_cast<King*>(piece)) {
				
			}
			*/

			cout << "From: " << playerMove.from.getAlgebraicNotation() << " To: " << playerMove.to.getAlgebraicNotation() << endl;
			board.queuePlayerMove(playerMove);
			currentTurn++;
		}
	}
	board.update();
}

void Game::updateState() {
	updateWaitFrames = 60;
	queuedForUpdate = false;
	// Check if the game ended
	int currentPlayer = getPlayerTurn();

	if (board.isInCheckmate(currentPlayer) || board.isInStalemate(currentPlayer)) {
		gameEnd = true;
		return;
	}

	// Update the state of the board
	board.updateState();

	// Update the state of the events
	for (auto& event : activeEvents) {
		event->updateState(board);
	}

	activeEvents.erase(
		remove_if(activeEvents.begin(), activeEvents.end(), [](Event* event) {
			return event->getLifetime() <= 0;
			}),
		activeEvents.end()
	);
}

void Game::updateMusicStreams() {

	int numberOfIceTiles      = board.getTileCount<IceTile>();
	int numberOfConveyorTiles = board.getTileCount<ConveyorTile>();
	int numberOfBreakTiles    = board.getTileCount<BreakingTile>();
	int numberOfPortalTiles   = board.getTileCount<PortalTile>();

	float iceVolume      = Clamp((float)numberOfIceTiles / 3.0f, 0.0f, 1.0f); // Reaches max volume at 3 ice tiles
	float conveyorVolume = Clamp((float)numberOfConveyorTiles / 8.0f, 0.0f, 1.0f); // Reaches max volume at 8 conveyor tiles
	float breakVolume    = Clamp((float)numberOfBreakTiles / 3.0f, 0.0f, 1.0f); // Reaches max volume at 3 break tiles
	float portalVolume   = Clamp((float)numberOfPortalTiles / 6.0f, 0.0f, 1.0f); // Reaches max volume at 6 portal tiles

	SetMusicVolume(musicIce, iceVolume);
	SetMusicVolume(musicConveyor, conveyorVolume);
	SetMusicVolume(musicBreak, breakVolume);
	SetMusicVolume(musicPortal, portalVolume);


	UpdateMusicStream(musicNormal);
	UpdateMusicStream(musicIce);
	UpdateMusicStream(musicBreak);
	UpdateMusicStream(musicConveyor);
	UpdateMusicStream(musicPortal);
}

void Game::setSelectedCell(Cell cell) {
	selectedCell = cell;
}

RenderQueue& Game::getRenderQueue() {
	return renderQueue;
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

Player& Game::getCurrentPlayer() {
	return getPlayer(getPlayerTurn());
}

Board& Game::getBoard() {
	return board;
}

int Game::getPlayerTurn() {
	return (currentTurn % 2) + 1;
}