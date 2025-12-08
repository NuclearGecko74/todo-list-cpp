#ifndef TODOLIST_APPLAYER_H
#define TODOLIST_APPLAYER_H

#include "BaseLayer.h"
#include "Core/Layer.h"
#include "Model/ListManager.h"

#include <vector>

class AppLayer : public BaseLayer {
private:
    enum class AppScreen {
        Tasks = 0,
        Settings,
        Calendar
    };
public:
    AppLayer();
    ~AppLayer() override = default;

    void OnUpdate(float ts) override;
    void OnRender() override;

private:
    void ReloadLists();
    void LoadTaskToBuffer(int taskId);

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

    AppScreen m_CurrentScreen = AppScreen::Tasks;

    std::vector<TaskListSpecification> m_Lists;
    std::unique_ptr<TaskManager> m_TaskManager;
    std::unique_ptr<ListManager> m_ListManager;

    int m_SelectedListIndex = -1;
    int m_SelectedTaskIndex = -1;

    std::string m_DayBuffer;
    std::string m_MonthBuffer;
    std::string m_YearBuffer;
    std::string m_EditTitleBuffer;
    std::string m_EditContentBuffer;
    std::string m_DisplayDateBuffer;

    int m_RenamingListIndex = -1; // -1 = renaming list
    std::string m_RenameListBuffer;
};

#endif