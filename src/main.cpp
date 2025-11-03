#include "raylib.h"

int main()
{
    constexpr int screenWidth = 800;
    constexpr int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "To-Do List");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("Toy chica es la unica animatronico, la cual lleva ropa la cual es un short, que la hace ver muy SECY", 0, 200, 20, BLACK);

        EndDrawing();
    }
    CloseWindow();

    return 0;
}