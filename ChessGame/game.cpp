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

void Game::draw(raylib::Texture2D* atlas) {
	theme.drawBackground();

	board.draw(theme, renderQueue, getPlayerTurn(), selectedCell);

	renderQueue.sortQueue();
	renderQueue.draw();
	renderQueue.clearQueue();

	EndMode2D();

	if (promotionMenu.has_value()) {
		promotionMenu.value().draw(atlas, board);
	}
}

void Game::update(raylib::Texture2D* atlas) {
	theme.updateBackground(); // Update the theme

	// Remove finished promotion menus
	if (promotionMenu.has_value()) {
		if (promotionMenu.value().finished()) {
			promotionMenu = nullopt;
		}
	}
	
	// Add a new promotion menu if needed
	if (board.hasPromotion() && !promotionMenu.has_value()) {
		Cell promotionCell = board.getPromotionCell();

		if (board.getPiece(promotionCell)->getPlayer() == 1) {
			promotionMenu = PromotionMenu(promotionCell);
		} else {
			// TODO: actually let the AI choose the piece to promote to, if it desires to underpromote for any reason
			board.getTile(promotionCell)->setPiece(new Queen(atlas, 2)); // Automatically promote to queen for player 2
		}
	}

	if (!promotionMenu) {
		if (!board.isPlayable()) {
			if (board.handlingStateUpdate) {
				updateState();
			}
		} else { // Wait for player to make a move
			Player& currentPlayer = getCurrentPlayer();

			// Check if the current player has made a move
			if (currentPlayer.hasMove()) {
				cout << endl << "Player #" << getPlayerTurn() << " has made a move!" << endl;
				Move playerMove = currentPlayer.getMove(); // Get the player's move
				// Can add extra failsafe handling here if needed, but not for now

				Piece* piece = board.getTile(playerMove.from)->getPiece(); // Get the piece the player is moving

				// Since castling is the only move that moves 2 pieces at the same time, i'm just handling it here.
				// It might be a good idea to test this with interactions though.

				// Handle specific move flags
				if (playerMove.flag.has_value()) {
					switch (playerMove.flag.value()) {
					case MoveFlag::CASTLE: { // King castling
						cout << "Castled!" << endl;
						int direction = (playerMove.to.file > playerMove.from.file) ? 1 : -1;

						// Rook starts on the edge of the board depending on direction
						int rookStartFile = (direction == 1) ? 7 : 0;
						int rookEndFile = playerMove.from.file + direction;

						Cell rookFrom(playerMove.from.rank, rookStartFile);
						Cell rookTo(playerMove.from.rank, rookEndFile);

						// Create and queue a rook move
						Move rookMove(rookFrom, rookTo, false); // or whatever animation you prefer
						board.queueMove(rookMove);
						board.clearEnPassantableCell();
						break;
					}
					case MoveFlag::EN_PASSANTABLE: { // Pawn moving two tiles
						cout << "Piece is en passantable!" << endl;
						board.setEnPassantableCell(playerMove.to); // Set the en passantable cell
						break;
					}
					case MoveFlag::EN_PASSANT: { // Pawn taking a pawn in en passant
						cout << "Player used en passant!" << endl;
						break;
					}
					case MoveFlag::PROMOTION: { // Pawm moving to be promoted
						cout << "Pawn is to be promoted!" << endl;
						board.clearEnPassantableCell();
						break;
					}
					}
				}
				else {
					board.clearEnPassantableCell();
				}

				board.queueMove(playerMove); // Queue the player's move up

				board.handlingPlayerTurn = true;
			}
		}
	}

	// TODO: fix this so i dont have to update it like this
	board.update(getPlayerTurn()); // Update the board
}

void Game::updateState() {
	updateWaitFrames = 60;
	queuedForUpdate = false;

	currentTurn++; // Next player's turn

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

void Game::setSelectedCell(Cell cell) { selectedCell = cell; }

bool Game::isPlayable() { return board.isPlayable() && !promotionMenu.has_value(); }

RenderQueue& Game::getRenderQueue() { return renderQueue; }

bool Game::getGameEnd() { return gameEnd; }

bool Game::playerIsInCheck(int player) { return board.isInCheck(player); }

Player& Game::getPlayer(int player) {
	if (player <= 0 || player > players.size()) {
		throw runtime_error("Player index out of bounds");
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