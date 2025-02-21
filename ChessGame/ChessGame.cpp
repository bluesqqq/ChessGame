#include <iostream>
#include <cmath>
#include "board.h"
#include "isometric.h"
#include "include/raylib-cpp.hpp"
#include "player.h"

using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 360;

Texture2D atlas;

void UpdateDrawFrame(Camera2D camera, Board board, vector<Player> players)
{
    BeginDrawing();

    BeginMode2D(camera);

    ClearBackground(Color{ 204, 230, 255, 255 });


    float time = GetTime(); // Get elapsed time
    raylib::Vector2 cursorPosition = GetMousePosition();

    raylib::Vector2 cursorIsoPosition = ScreenToISO(cursorPosition - camera.offset);

    board.draw((int)cursorIsoPosition.x, (int)cursorIsoPosition.y);

    EndMode2D();

    DrawText(players[0].getName().c_str(), 10, 30, 20, BLACK);

    vector<Piece*> p1DiscardedPieces = players[0].getDiscardedPieces();
    for (int i = 0; i < p1DiscardedPieces.size(); i++) {
        p1DiscardedPieces[i]->drawIcon(0 + (i % 2) * 32, 50 + (16 * i));
    }

    DrawText(players[1].getName().c_str(), 640 - MeasureText(players[1].getName().c_str() - 10, 20), 30, 20, BLACK);

    vector<Piece*> p2DiscardedPieces = players[1].getDiscardedPieces();
    for (int i = 0; i < p2DiscardedPieces.size(); i++) {
        p2DiscardedPieces[i]->drawIcon(SCREEN_WIDTH - 64 + (i % 2) * 32 - 32, 50 + (16 * i));
    }

    EndDrawing();
}

Vector2 targetX;

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
    vector<Player> players;

    players.push_back(Player("Player 1"));
    players.push_back(Player("Player 2"));

    Board board = Board(&atlas, players);

    Camera2D camera = { 0 };
    camera.target = raylib::Vector2{ 0.0f, 0.0f };
    camera.offset = raylib::Vector2{ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 8.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    while (WindowShouldClose() == false)
    {
        UpdateMusicStream(music);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            PlaySound(fxPickup);

            Vector2 position = CursorToISO(camera);

            targetX = position;
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            PlaySound(fxPutdown);

            Vector2 position = CursorToISO(camera);

            board.MovePiece(targetX.x, targetX.y, position.x, position.y);
        }

        UpdateDrawFrame(camera, board, players);
    }

    CloseWindow();

    return 0;
}