#include "DebugLayer.h"

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

static bool showMessageBox;

DebugLayer::DebugLayer()
{
    showMessageBox = false;
}

DebugLayer::~DebugLayer()
{
}

void DebugLayer::OnEvent(Event &event)
{
    Layer::OnEvent(event);
}

void DebugLayer::OnUpdate(float ts)
{
    Layer::OnUpdate(ts);
}

void DebugLayer::OnRender()
{
    Layer::OnRender();
    if (GuiButton({ 24, 24, 240, 60 }, "#191#DEBUG BUTTON")) showMessageBox = true;

    if (showMessageBox)
    {
        int result = GuiMessageBox({ 85, 70, 500, 200 },
            "#191#Message Box", "Hi! YOU ARE IN DEBUG MODE!", "WAZA;67");

        if (result >= 0) showMessageBox = false;
    }
}
