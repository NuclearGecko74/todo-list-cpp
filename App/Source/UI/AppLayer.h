#ifndef TODOLIST_APPLAYER_H
#define TODOLIST_APPLAYER_H

#include "BaseLayer.h"
#include "Core/Layer.h"

class AppLayer : public BaseLayer {
private:
    enum class AppScreen {
        Tasks = 0,
        Browse,
        Settings,
        Calendar
    };
public:
    AppLayer() = default;
    ~AppLayer() override = default;

    void OnUpdate(float ts) override;
    void OnRender() override;

private:
    void RenderSidebar(Rectangle bounds);
    void RenderTopBar(Rectangle bounds);

    void RenderContentArea(Rectangle bounds);

    void RenderTasksScreen(Rectangle bounds);
    void RenderBrowseScreen(Rectangle bounds);
    void RenderCalendarScreen(Rectangle bounds);
    void RenderSettingsScreen(Rectangle bounds);

    void DrawTaskItem(Rectangle bounds, const char* text, bool completed, int index);
    bool DrawNavButton(const char* text, int iconId, bool isSelected, Rectangle bounds);

private:
    // Layout Constants
    static constexpr float SIDEBAR_WIDTH = 250.0f;
    static constexpr float TOP_BAR_HEIGHT = 100.0f;
    static constexpr float PADDING = 20.0f;

private:
    Vector2 m_TaskScrollOffset = { 0, 0 };
    Rectangle m_TaskContentRect = { 0, 0, 0, 0 };

    int m_SelectedTaskIndex = -1;

    AppScreen m_CurrentScreen = AppScreen::Tasks;
};


#endif