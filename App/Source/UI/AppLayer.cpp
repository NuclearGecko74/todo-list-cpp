#include "AppLayer.h"

#include "Core/Application.h"

#include "raylib.h"
#include "raygui.h"
#include "UI/Theme.h"

// Logic updates
void AppLayer::OnUpdate(float ts)
{
    Layer::OnUpdate(ts);
    // TODO Sync with db
}

void AppLayer::OnRender()
{
    Layer::OnRender();

    // 1. Calculate Layout (Dynamic)
    const auto screenW = static_cast<float>(GetScreenWidth());
    const auto screenH = static_cast<float>(GetScreenHeight());

    Rectangle sidebarRect = {
        0, 0,
        SIDEBAR_WIDTH,
        screenH
    };

    Rectangle topBarRect = {
        SIDEBAR_WIDTH, 0,
        screenW - SIDEBAR_WIDTH,
        TOP_BAR_HEIGHT
    };

    Rectangle contentRect = {
        SIDEBAR_WIDTH, TOP_BAR_HEIGHT,
        screenW - SIDEBAR_WIDTH,
        screenH - TOP_BAR_HEIGHT
    };

    // 2. Clear Main Background
    ClearBackground(Theme::BG_Main);

    // 3. Delegate Drawing
    RenderSidebar(sidebarRect);
    RenderTopBar(topBarRect);
    RenderContentArea(contentRect);
}

void AppLayer::RenderSidebar(Rectangle bounds)
{
    // Background
    DrawRectangleRec(bounds, Theme::BG_Sidebar);

    // Title
    DrawLabel(
        Core::Application::Get().GetName().c_str(),
        static_cast<int>(bounds.x + PADDING),
        static_cast<int>(bounds.y + PADDING),
        40,
        Theme::Text_Light
    );

    // Subtitle
    DrawLabel(
        "Discover",
        static_cast<int>(bounds.x + PADDING),
        static_cast<int>(bounds.y + 80),
        20,
        Theme::Text_Light
    );

    struct NavItem {
        const char* Label;
        int IconId;
        AppScreen Screen;
    };

    NavItem items[] = {
        { "Tasks",    ICON_HOUSE,    AppScreen::Tasks },
        { "Browse",   ICON_FOLDER_OPEN, AppScreen::Browse },
        { "Calendar", ICON_PLAYER_NEXT, AppScreen::Calendar },
        { "Settings", ICON_GEAR,     AppScreen::Settings }
    };

    float buttonY = bounds.y + 120.0f;
    float buttonHeight = 50.0f;
    float buttonWidth = bounds.width - (PADDING * 2);

    for (const auto& item : items)
    {
        Rectangle btnRect = {
            bounds.x + PADDING,
            buttonY,
            buttonWidth,
            buttonHeight
        };

        bool isSelected = (m_CurrentScreen == item.Screen);

        if (DrawNavButton(item.Label, item.IconId, isSelected, btnRect))
        {
            m_CurrentScreen = item.Screen;
        }

        buttonY += buttonHeight + 10.0f;
    }
}

void AppLayer::RenderTopBar(Rectangle bounds)
{
    Rectangle container = {
        bounds.x + PADDING,
        bounds.y + PADDING,
        bounds.width - (PADDING * 2),
        bounds.height - (PADDING * 2)
    };

    // Draw Search Bar Area
    DrawRectangleRounded(container, 0.5f, 10, Theme::Border_Panel);
}

void AppLayer::RenderContentArea(Rectangle bounds)
{
    DrawRectangleRec(bounds, Theme::BG_Main);

    switch (m_CurrentScreen)
    {
    case AppScreen::Tasks:
        RenderTasksScreen(bounds);
        break;
    case AppScreen::Browse:
        RenderBrowseScreen(bounds);
        break;
    case AppScreen::Calendar:
        RenderCalendarScreen(bounds);
        break;
    case AppScreen::Settings:
        RenderSettingsScreen(bounds);
        break;
    }
}

bool AppLayer::DrawNavButton(const char *text, int iconId, bool isSelected, Rectangle bounds)
{
    int defaultTextColor = GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL);
    int defaultBaseColor = GuiGetStyle(BUTTON, BASE_COLOR_NORMAL);

    if (isSelected)
    {
        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(Theme::BG_Panel));
        GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(Theme::Text_Dark));
        GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(Theme::BG_Panel));
    }
    else
    {
        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(Theme::BG_Sidebar));
        GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(Theme::Text_Light));
        GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(Theme::BG_Sidebar));
    }

    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    GuiSetStyle(BUTTON, TEXT_PADDING, 40);

    bool clicked = GuiButton(bounds, GuiIconText(iconId, text));

    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiSetStyle(BUTTON, TEXT_PADDING, 0);
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, defaultBaseColor);
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, defaultTextColor);

    if (isSelected)
    {
        DrawRectangleRounded(
            { bounds.x - 10, bounds.y + 5, 5, bounds.height - 10 },
            1.0f, 4,
            Theme::Text_Dark
        );
    }

    return clicked;
}

void AppLayer::RenderTasksScreen(Rectangle bounds)
{
    DrawLabel("Tasks View - Coming Soon", (int)bounds.x + 50, (int)bounds.y + 50, 40, Theme::Text_Dark);
}

void AppLayer::RenderBrowseScreen(Rectangle bounds)
{
    DrawLabel("Browse View - Coming Soon", (int)bounds.x + 50, (int)bounds.y + 50, 40, Theme::Text_Dark);
}

void AppLayer::RenderCalendarScreen(Rectangle bounds)
{
    DrawLabel("Calendar View - Coming Soon", (int)bounds.x + 50, (int)bounds.y + 50, 40, Theme::Text_Dark);
}

void AppLayer::RenderSettingsScreen(Rectangle bounds)
{
    DrawLabel("Settings View - Coming Soon", (int)bounds.x + 50, (int)bounds.y + 50, 40, Theme::Text_Dark);
}