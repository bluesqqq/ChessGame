#include "raylib.h"
#include <iostream>
#include <cmath>
#include "board.h"
#include "isometric.h"

using namespace std;

Texture2D atlas;

void UpdateDrawFrame(Camera2D camera, Board board)
{
    BeginDrawing();

    BeginMode2D(camera);

    ClearBackground(RAYWHITE);

    board.draw();

    float time = GetTime(); // Get elapsed time
    Vector2 cursorPosition = GetMousePosition();

    Vector2 cursorIsoPosition = ScreenToISO(cursorPosition);

    EndMode2D();

    EndDrawing();
}

int main()
{
    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 360;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Chess");
    SetTargetFPS(60);

    atlas = LoadTexture("resources/Tiles.png");

    Board board = Board(&atlas);

    Camera2D camera = { 0 };
    camera.target = Vector2{ 0.0f, 0.0f };
    camera.offset = Vector2{ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 8.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    while (WindowShouldClose() == false)
    {
        UpdateDrawFrame(camera, board);
    }

    CloseWindow();

    return 0;
}