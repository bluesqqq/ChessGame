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

void UpdateDrawFrame(Camera2D camera, Game game)
{
    BeginDrawing();

    BeginMode2D(camera);

    ClearBackground(Color{ 204, 230, 255, 255 });


    float time = GetTime(); // Get elapsed time
    raylib::Vector2 cursorPosition = GetMousePosition();

    raylib::Vector2 cursorIsoPosition = ScreenToISO(cursorPosition - camera.offset);

    game.getBoard().draw(game.getPlayerTurn(), (int)cursorIsoPosition.x, (int)cursorIsoPosition.y);

    EndMode2D();

    DrawText(game.getPlayer(1).getName().c_str(), 10, 30, 20, BLACK);
    std::string p1check = game.playerIsInCheck(1) ? "true" : "false";
    DrawText(p1check.c_str(), 100, 10, 20, BLACK);

    vector<Piece*> p1DiscardedPieces = game.getPlayer(1).getDiscardedPieces();
    for (int i = 0; i < p1DiscardedPieces.size(); i++) {
        p1DiscardedPieces[i]->drawIcon(0 + (i % 2) * 32, 50 + (16 * i));
    }

    DrawText(game.getPlayer(2).getName().c_str(), 640 - MeasureText(game.getPlayer(2).getName().c_str() - 10, 20), 30, 20, BLACK);
    std::string p2check = game.playerIsInCheck(2) ? "true" : "false";
    DrawText(p2check.c_str(), 200, 10, 20, BLACK);

    vector<Piece*> p2DiscardedPieces = game.getPlayer(2).getDiscardedPieces();
    for (int i = 0; i < p2DiscardedPieces.size(); i++) {
        p2DiscardedPieces[i]->drawIcon(SCREEN_WIDTH - 64 + (i % 2) * 32 - 32, 50 + (16 * i));
    }

    EndDrawing();
}

Vector2 target;

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

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            PlaySound(fxPickup);

            Vector2 position = CursorToISO(camera);

            target = position;
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            PlaySound(fxPutdown);

            Vector2 position = CursorToISO(camera);

            game.movePiece(target.x, target.y, position.x, position.y);
        }

        UpdateDrawFrame(camera, game);

        game.update();

        exitWindow = game.getGameEnd();
    }

    CloseWindow();

    return 0;
}