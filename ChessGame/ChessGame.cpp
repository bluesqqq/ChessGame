#include <iostream>
#include <cmath>
#include "board.h"
#include "isometric.h"
#include "include/raylib-cpp.hpp"
#include "player.h"
#include "game.h"
#include "MoveGenerator.h"

using namespace std;

const int SCREEN_WIDTH = 768;
const int SCREEN_HEIGHT = 432;

raylib::Texture2D* atlas;

raylib::Vector2* target = nullptr;
raylib::Vector3 interpolatedCursorIsoPositionFloat = { 0.0f, 0.0f, 0.0f };

Tile* selectedTile = nullptr;
Piece* selectedPiece = nullptr;

void UpdateDrawFrame(Camera2D camera, Game& game) {
    float time = GetTime(); // Get elapsed time

    Board& board = game.getBoard();

    BeginDrawing();

    BeginMode2D(camera);

    raylib::Vector2 cursorPosition = GetMousePosition(); // Get the cursor position

    raylib::Vector3 cursorIsoPositionFloat = ScreenToISOFloat(cursorPosition - camera.offset, 2.0f);

    interpolatedCursorIsoPositionFloat = interpolatedCursorIsoPositionFloat.Lerp(cursorIsoPositionFloat, 0.2f);

    if (selectedTile) {
        game.setSelectedCell(board.getTileCell(selectedTile));
    } else {
        game.setSelectedCell(board.getCellAtScreenPosition(cursorPosition, camera));
    }

    game.draw();

    if (selectedPiece) {
        selectedPiece->draw(game.getRenderQueue(), interpolatedCursorIsoPositionFloat.x, interpolatedCursorIsoPositionFloat.y, interpolatedCursorIsoPositionFloat.z); // Adjust the offset as needed
    }

    EndMode2D();

    /*
        DRAW UI
    */

    string move = game.getPlayer(game.getPlayerTurn()).getName() + " to move";

    DrawText(move.c_str(), (SCREEN_WIDTH / 2) - (MeasureText(move.c_str(), 20) / 2), SCREEN_HEIGHT - 30, 20, WHITE);

    DrawText(game.getPlayer(1).getName().c_str(), 10, 30, 20, WHITE);

    vector<Piece*> p1DiscardedPieces = game.getPlayer(1).getDiscardedPieces();
    for (int i = 0; i < p1DiscardedPieces.size(); i++) {
        p1DiscardedPieces[i]->drawIcon(0 + (i % 2) * 32, 50 + (16 * i));
    }

    DrawText(game.getPlayer(2).getName().c_str(), 640 - MeasureText(game.getPlayer(2).getName().c_str() - 10, 20), 30, 20, WHITE);

    vector<Piece*> p2DiscardedPieces = game.getPlayer(2).getDiscardedPieces();
    for (int i = 0; i < p2DiscardedPieces.size(); i++) {
        p2DiscardedPieces[i]->drawIcon(SCREEN_WIDTH - 64 + (i % 2) * 32 - 32, 50 + (16 * i));
    }

    EndDrawing();
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Chess");
    
    InitAudioDevice();

    SetTargetFPS(60);

    atlas = new raylib::Texture2D(LoadTexture("resources/Tiles.png"));

    Game game = Game(atlas);

    Camera2D camera = { 0 };
    camera.target = raylib::Vector2{ 0.0f, 0.0f };
    camera.offset = raylib::Vector2{ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 8.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    bool exitWindow = false;

    float HALF_SCREEN_WIDTH = SCREEN_WIDTH / 2.0f;
    float HALF_SCREEN_HEIGHT = SCREEN_HEIGHT / 2.0f;

    float mouseOffsetMultiplier = 15;

    raylib::Vector2 cameraMouseOffset = { 0.0f, 0.0f };

    /*
        LOOP
    */ 
    while (!exitWindow && !WindowShouldClose()) {
        game.updateMusicStreams();

		Board& board = game.getBoard();

        raylib::Vector2 mousePosition = GetMousePosition();

        cameraMouseOffset = cameraMouseOffset.Lerp(raylib::Vector2(((mousePosition.x - HALF_SCREEN_WIDTH) / HALF_SCREEN_WIDTH) * mouseOffsetMultiplier, ((mousePosition.y - HALF_SCREEN_HEIGHT) / HALF_SCREEN_HEIGHT) * mouseOffsetMultiplier), 0.02f);

        camera.target = cameraMouseOffset;

        Player& currentPlayer = game.getCurrentPlayer();

        if (!game.queuedForUpdate) {

            if (game.getBoard().isPlayable()) {
                
                if (game.getPlayerTurn() == 1) {

                    // LEFT CLICK
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        Tile* targetTile = board.getTile(board.getCellAtScreenPosition(mousePosition, camera));

                        // Check if tile exists
                        if (targetTile) {
                            Piece* targetPiece = targetTile->getPiece();

                            // Check if piece exists on tile and if it is the current player's
                            if (targetPiece && targetPiece->isSelectable() && targetPiece->getPlayer() == game.getPlayerTurn() && !targetPiece->getLegalMoves(game.getBoard()).empty()) {
                                selectedTile = targetTile;
                                selectedPiece = targetPiece;

								Vector2 position = game.getBoard().getTilePosition(selectedTile);

                                interpolatedCursorIsoPositionFloat = { position.x, position.y, 0.0f };

                                Sound fxPickup = LoadSound("resources/pickup.wav");
                                PlaySound(fxPickup);
                            }
                        }
                    }

                    /* This looks ugly at low resolution
                    if (selectedPiece) {
					    camera.zoom = Lerp(camera.zoom, 1.2f, 0.1f);
                    } else {
                        camera.zoom = Lerp(camera.zoom, 1.0f, 0.1f);
                    }
                    */

                    // LEFT RELEASE
                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && (selectedTile)) {
                        Tile* destinationTile = board.getTile(board.getCellAtScreenPosition(mousePosition, camera));

                        // Check if tile exists
                        if (destinationTile) {

                            // Check if the move is valid
                            Vector2 selectedTilePosition = game.getBoard().getTilePosition(selectedTile);
                            Vector2 destinationTilePosition = game.getBoard().getTilePosition(destinationTile);

							Cell selectedCell    = Cell(game.getBoard().getTileCell(selectedTile));
                            Cell destinationCell = Cell(game.getBoard().getTileCell(destinationTile));

                            if (game.getBoard().isLegalMove(game.getPlayerTurn(), selectedCell, destinationCell)) {
                                Move move = Move{ destinationTile, selectedTile, true, createPickAndPlaceAnimation({0, 0, 0}, { destinationTilePosition.x - selectedTilePosition.x, destinationTilePosition.y - selectedTilePosition.y, 0 }) };

                                cout << "Setting Move: From: " << game.getBoard().getTilePositionName(move.from) << " To: " << game.getBoard().getTilePositionName(move.to) << endl;
                                currentPlayer.setMove(move);
                            }
                        }

                        selectedTile = nullptr;
                        selectedPiece = nullptr;

                        Sound fxPutdown = LoadSound("resources/putdown.wav");

                        PlaySound(fxPutdown);
                    }

                } else { // AI's turn
                    MoveGenerator generator = MoveGenerator(game);

                    TileMove tileMove = generator.chooseMove(game.getPlayerTurn(), 5);

                    Move move = Move(game.getBoard().getTile(tileMove.toRow, tileMove.toCol), game.getBoard().getTile(tileMove.fromRow, tileMove.fromCol), true, createPickAndPlaceAnimation({ 0, 0, 0 }, { (float)(tileMove.toRow - tileMove.fromRow), (float)(tileMove.toCol - tileMove.fromCol), 0 }));

                    currentPlayer.setMove(move);
                    /*

                    vector<Move> moves = game.getBoard().getAllLegalMoves(game.getPlayerTurn());

                    // Right now it just picks random moves
                    if (!moves.empty()) {

                        vector<Move> overtakes;

                        for (Move& move : moves) {
                            if (move.overtakes()) {
                                overtakes.push_back(move);
                            }
                        }

                        if (!overtakes.empty()) {
                            int index = rand() % overtakes.size();

                            Move selectedMove = Move(overtakes[index]);

                            // Pass pointer to setMove
                            currentPlayer.setMove(selectedMove);
                        }
                        else {
                            int index = rand() % moves.size();

                            Move selectedMove = Move(moves[index]);

                            // Pass pointer to setMove
                            currentPlayer.setMove(selectedMove);
                        }
                    }
                    */
                }
            }
        } else {
            if (game.updateWaitFrames == 0) {
                game.updateState();
            }
            else {
                game.updateWaitFrames--;
            }
        }

        game.update();

        UpdateDrawFrame(camera, game);

        exitWindow = game.getGameEnd();
    }

    CloseWindow();

    return 0;
}