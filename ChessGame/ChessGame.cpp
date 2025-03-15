#include <iostream>
#include <cmath>
#include "board.h"
#include "isometric.h"
#include "include/raylib-cpp.hpp"
#include "player.h"
#include "game.h"

using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 360;

Texture2D atlas;

raylib::Vector2* target = nullptr;
raylib::Vector3 interpolatedCursorIsoPositionFloat = { 0.0f, 0.0f, 0.0f };

Tile* selectedTile = nullptr;
Piece* selectedPiece = nullptr;

void UpdateDrawFrame(Camera2D camera, Game& game)
{
    float time = GetTime(); // Get elapsed time

    BeginDrawing();

    BeginMode2D(camera);

    ClearBackground(Color{ 204, 230, 255, 255 });

    raylib::Vector2 cursorPosition = GetMousePosition();

    raylib::Vector2 cursorIsoPosition = ScreenToISO(cursorPosition - camera.offset);

    raylib::Vector2 cursorIsoPositionFloat = ScreenToISOFloat(cursorPosition - camera.offset);

    interpolatedCursorIsoPositionFloat = interpolatedCursorIsoPositionFloat.Lerp({ cursorIsoPositionFloat.x, cursorIsoPositionFloat.y, 2.0f }, 0.2f);

    if (selectedTile) cursorIsoPosition = game.getBoard().getTilePosition(selectedTile);

    game.getBoard().draw(game.getPlayerTurn(), (int)cursorIsoPosition.x, (int)cursorIsoPosition.y);

    // Draw the selected piece at the mouse position
    if (selectedPiece) {
        selectedPiece->draw(interpolatedCursorIsoPositionFloat.x, interpolatedCursorIsoPositionFloat.y, interpolatedCursorIsoPositionFloat.z); // Adjust the offset as needed
    }

    EndMode2D();

    DrawText(game.getPlayer(1).getName().c_str(), 10, 30, 20, BLACK);

    vector<Piece*> p1DiscardedPieces = game.getPlayer(1).getDiscardedPieces();
    for (int i = 0; i < p1DiscardedPieces.size(); i++) {
        p1DiscardedPieces[i]->drawIcon(0 + (i % 2) * 32, 50 + (16 * i));
    }

    DrawText(game.getPlayer(2).getName().c_str(), 640 - MeasureText(game.getPlayer(2).getName().c_str() - 10, 20), 30, 20, BLACK);

    vector<Piece*> p2DiscardedPieces = game.getPlayer(2).getDiscardedPieces();
    for (int i = 0; i < p2DiscardedPieces.size(); i++) {
        p2DiscardedPieces[i]->drawIcon(SCREEN_WIDTH - 64 + (i % 2) * 32 - 32, 50 + (16 * i));
    }

    EndDrawing();
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Chess");
    
    InitAudioDevice();

    SetTargetFPS(60);

    atlas = LoadTexture("resources/Tiles.png");

    Game game = Game(&atlas);

    Camera2D camera = { 0 };
    camera.target = raylib::Vector2{ 0.0f, 0.0f };
    camera.offset = raylib::Vector2{ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 8.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    bool exitWindow = false;

    while (!exitWindow && !WindowShouldClose())
    {
        game.updateMusicStreams();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            raylib::Vector2 position = CursorToISO(camera);
            Tile* targetTile = game.getBoard().getTile(position.x, position.y);

            // Check if tile exists
            if (targetTile) {
                Piece* targetPiece = targetTile->getPiece();

                // Check if piece exists on tile and if it is the current player's
                if (targetPiece && targetPiece->getPlayer() == game.getPlayerTurn()) {
                    selectedTile = targetTile;
                    selectedPiece = targetPiece;

                    interpolatedCursorIsoPositionFloat = { position.x, position.y, 0.0f };

                    Sound fxPickup = LoadSound("resources/pickup.wav");
                    PlaySound(fxPickup);
                }
            }
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && (selectedTile)) {
            raylib::Vector2 position = CursorToISO(camera);
            Tile* destinationTile = game.getBoard().getTile(position.x, position.y);

            // Check if tile exists
            if (destinationTile) {
                Vector2 selectedTilePosition    = game.getBoard().getTilePosition(selectedTile);
                Vector2 destinationTilePosition = game.getBoard().getTilePosition(destinationTile);

                // Move target piece to destination tile
                game.movePiece(selectedTilePosition.x, selectedTilePosition.y, destinationTilePosition.x, destinationTilePosition.y);

            }

            selectedTile = nullptr;
            selectedPiece = nullptr;

            Sound fxPutdown = LoadSound("resources/putdown.wav");

            PlaySound(fxPutdown);
        }

        UpdateDrawFrame(camera, game);

        exitWindow = game.getGameEnd();
    }

    CloseWindow();

    return 0;
}