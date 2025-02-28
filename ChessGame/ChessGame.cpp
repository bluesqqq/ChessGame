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

Vector2* target = nullptr;

void UpdateDrawFrame(Camera2D camera, Game game)
{
    float time = GetTime(); // Get elapsed time

    BeginDrawing();

    BeginMode2D(camera);

    ClearBackground(Color{ 204, 230, 255, 255 });

    raylib::Vector2 cursorPosition = GetMousePosition();

    raylib::Vector2 cursorIsoPosition = ScreenToISO(cursorPosition - camera.offset);

    if (target) cursorIsoPosition = *target;

    game.getBoard().draw(game.getPlayerTurn(), (int)cursorIsoPosition.x, (int)cursorIsoPosition.y);

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

    Sound fxPickup = LoadSound("resources/pickup.wav");
    Sound fxPutdown = LoadSound("resources/putdown.wav");

    Music music = LoadMusicStream("resources/music.wav");

    SetMusicVolume(music, 0.1f);
    PlayMusicStream(music);

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
        UpdateMusicStream(music);

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            Vector2 position = CursorToISO(camera);

            if (target == nullptr) { // No target selected
                Tile* targetTile = game.getBoard().getTile(position.x, position.y);

                if (targetTile) {
                    if (targetTile->hasPiece()) {
                        if (targetTile->getPiece()->getPlayer() == game.getPlayerTurn()) {
                            target = new Vector2(position);

                            PlaySound(fxPickup);
                        }
                    }
                }
            } else {
                Tile* targetTile = game.getBoard().getTile(position.x, position.y);

                if (targetTile) {
                    if (targetTile->hasPiece()) {
                        if (targetTile->getPiece()->getPlayer() == game.getPlayerTurn()) {
                            PlaySound(fxPickup);
                            target = new Vector2(position);
                        } else {
                            PlaySound(fxPutdown);
                            game.movePiece(target->x, target->y, position.x, position.y);
                            target = nullptr;
                        }
                    } else {
                        PlaySound(fxPutdown);
                        game.movePiece(target->x, target->y, position.x, position.y);
                        target = nullptr;
                    }
                }
                else {
                    PlaySound(fxPutdown);
                    target = nullptr;
                }
            }
        }

        UpdateDrawFrame(camera, game);

        exitWindow = game.getGameEnd();
    }

    CloseWindow();

    return 0;
}