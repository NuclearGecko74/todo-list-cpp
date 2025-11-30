#include "DebugLayer.h"

void DebugLayer::OnRender()
{
    Layer::OnRender();

    constexpr float kBoxWidth = 250.0f;
    constexpr float kBoxHeight = 110.0f;
    constexpr float kPadding = 10.0f;

    const float screenW = static_cast<float>(GetScreenWidth());
    const float screenH = static_cast<float>(GetScreenHeight());

    const float boxX = screenW - kBoxWidth - kPadding;
    const float boxY = screenH - kBoxHeight - kPadding;

    DrawRectangle(static_cast<int>(boxX), static_cast<int>(boxY), static_cast<int>(kBoxWidth),
        static_cast<int>(kBoxHeight), Fade(BLACK, 0.5f));
    DrawRectangleLines(static_cast<int>(boxX), static_cast<int>(boxY), static_cast<int>(kBoxWidth),
        static_cast<int>(kBoxHeight), GREEN);

    const int fps = GetFPS();
    const float dt = GetFrameTime();
    const Vector2 mouse = GetMousePosition();

    const Color fpsColor = (fps >= 60) ? LIME : RED;

    DrawText(TextFormat("%d FPS (%.2f ms)", fps, dt * 1000.0f),
             static_cast<int>(boxX + 10), static_cast<int>(boxY + 10), 20, fpsColor);

    DrawText(TextFormat("Mouse: [%.0f, %.0f]", mouse.x, mouse.y),
             static_cast<int>(boxX + 10), static_cast<int>(boxY + 40), 10, WHITE);

    DrawText(TextFormat("Window: %.0fx%.0f", screenW, screenH),
             static_cast<int>(boxX + 10), static_cast<int>(boxY + 60), 10, WHITE);

    DrawText("Build: DEBUG MODE",
             static_cast<int>(boxX + 10), static_cast<int>(boxY + 80), 10, YELLOW);
}