#include "AppLayer.h"

#include "Core/Application.h"

#include "raylib.h"
#include "raygui.h"

#include "UI/Theme.h"

void AppLayer::OnEvent(Event &event)
{
    Layer::OnEvent(event);
}

void AppLayer::OnUpdate(float ts)
{
    Layer::OnUpdate(ts);
}

void AppLayer::OnRender()
{
    Layer::OnRender();

    const auto screenW = static_cast<float>(GetScreenWidth());
    const auto screenH = static_cast<float>(GetScreenHeight());

    RenderBackground();
}

void AppLayer::RenderBackground()
{
    ClearBackground(Theme::BG_Main);
    DrawRectangle(0, 0, 300, GetScreenHeight(),Theme::BG_Sidebar);

    DrawRectangleRounded({330, 20, 1560, 70},
        .8f, 10,
        Theme::Border_Panel
        );

    DrawLabel(
        Core::Application::Get().GetName().c_str(),
        20, 20, 50,
        Theme::Text_Light
        );
}

void AppLayer::RenderNavigationBar()
{

}