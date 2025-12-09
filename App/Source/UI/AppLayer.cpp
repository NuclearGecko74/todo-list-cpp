#include "AppLayer.h"

#include <sstream>

#include "AuthLayer.h"
#include "Core/Application.h"

#include "raylib.h"
#include "raygui.h"
#include "UI/Theme.h"
#include "Model/Utilities.h"

#include <ctime>
#include <iomanip>

AppLayer::AppLayer()
{
    m_ListManager= std::make_unique<ListManager>(AppResources::GetDatabase());
    m_TaskManager = std::make_unique<TaskManager>(AppResources::GetDatabase());
    ReloadLists();

    m_SettingsLogo = LoadTexture("assets/full_icon.png");
    SetTextureFilter(m_SettingsLogo, TEXTURE_FILTER_BILINEAR);
}

AppLayer::~AppLayer()
{
    UnloadTexture(m_SettingsLogo);
}

void AppLayer::ReloadLists()
{
    m_Lists = m_ListManager->loadLists(AppResources::GetUserId());

    if (m_Lists.empty()) {
        m_SelectedListIndex = -1;
    }
    else {
        if (m_SelectedListIndex == -1 || m_SelectedListIndex >= m_Lists.size()) {
            m_SelectedListIndex = 0;
        }
    }
}

void AppLayer::LoadTaskToBuffer(int taskId)
{
    if (m_SelectedListIndex == -1) return;
    int listId = m_Lists[m_SelectedListIndex].Id;
    auto tasks = m_TaskManager->loadTasks(listId);

    for (const auto& task : tasks)
    {
        if (task.getId() == taskId)
        {
            m_EditTitleBuffer = task.getTitle();
            m_EditContentBuffer = task.getDescription();

            // --- LÓGICA DE FECHA (SEPARAR) ---
            auto dueDate = task.getDueDate();
            auto timeT = std::chrono::system_clock::to_time_t(dueDate);

            // Si es fecha válida (> 1970)
            if (timeT > 10000)
            {
                std::tm tm = {};
                // Usamos tu función safe_localtime o localtime_s estándar
#if defined(_WIN32)
                localtime_s(&tm, &timeT);
#else
                localtime_r(&timeT, &tm);
#endif

                // Rellenar buffers (agregando '0' si es menor a 10 para estética)
                m_DayBuffer   = std::to_string(tm.tm_mday);
                m_MonthBuffer = std::to_string(tm.tm_mon + 1); // Mes es 0-11
                m_YearBuffer  = std::to_string(tm.tm_year + 1900);
            }
            else
            {
                // Limpiar si no hay fecha
                m_DayBuffer = "";
                m_MonthBuffer = "";
                m_YearBuffer = "";
            }
            return;
        }
    }
}

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
        { "Calendar", ICON_PLAYER_NEXT, AppScreen::Calendar },
        { "Settings", ICON_GEAR,     AppScreen::Settings }
    };

    float buttonY = bounds.y + 120.0f;
    float buttonHeight = 50.0f;
    float buttonWidth = bounds.width - (PADDING * 2);

    for (const auto& item : items)      // item class with button values
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
        bounds.x + PADDING * 2,
        bounds.y + PADDING,
        bounds.width - (PADDING * 6),
        bounds.height - (PADDING * 2)
    };
    DrawRectangleRounded(container, 0.5f, 10, Theme::Border_Panel);

    if (m_CurrentScreen != AppScreen::Tasks) return;

    float actionBtnWidth = 100.0f;
    float actionBtnHeight = 40.0f;
    float btnSpacing = 10.0f;
    float rightSidePadding = 20.0f;

    // Ajustado para 2 botones
    float rightSideTotalWidth = (2 * actionBtnWidth) + (1 * btnSpacing) + rightSidePadding;
    float fixedControlsWidth = 80.0f;

    Rectangle scrollArea = {
        container.x + 10,
        container.y,
        container.width - rightSideTotalWidth - fixedControlsWidth - 20,
        container.height
    };

    Rectangle fixedControlsArea = {
        scrollArea.x + scrollArea.width,
        container.y,
        fixedControlsWidth,
        container.height
    };

    static float listScrollX = 0.0f;
    float listTabWidth = 150.0f;
    float listTabHeight = container.height * 0.65f;
    float listSpacing = 10.0f;
    float totalContentWidth = m_Lists.size() * (listTabWidth + listSpacing);

    if (CheckCollisionPointRec(GetMousePosition(), scrollArea))
    {
        float wheel = GetMouseWheelMove();
        if (wheel != 0)
        {
            listScrollX -= wheel * 30.0f;
            float maxScroll = totalContentWidth - scrollArea.width;
            if (maxScroll < 0) maxScroll = 0;
            if (listScrollX < 0) listScrollX = 0;
            if (listScrollX > maxScroll) listScrollX = maxScroll;
        }
    }

    BeginScissorMode((int)scrollArea.x, (int)scrollArea.y, (int)scrollArea.width, (int)scrollArea.height);

    float currentX = scrollArea.x + 10 - listScrollX;
    float centerY = container.y + (container.height - listTabHeight) * 0.5f;

    for (int i = 0; i < m_Lists.size(); i++)
    {
        Rectangle listRect = { currentX, centerY, listTabWidth, listTabHeight };
        bool isSelected = (i == m_SelectedListIndex);
        Color tabColor = isSelected ? Theme::Selection_Blue : RAYWHITE;
        Color textColor = isSelected ? WHITE : BLACK;

        DrawRectangleRounded(listRect, 0.4f, 8, tabColor);

        if (m_RenamingListIndex == i)
        {
            Rectangle inputRect = { listRect.x + 5, listRect.y + 5, listRect.width - 10, listRect.height - 10 };
            DrawRectangleRec(inputRect, WHITE);
            DrawRectangleLinesEx(inputRect, 2, Theme::Selection_Blue);
            DrawLabel(m_RenameListBuffer.c_str(), inputRect.x + 5, inputRect.y + 8, 20, BLACK);

            int c;
            while ((c = GetCharPressed()) > 0) {
                if (c >= 32 && c <= 125 && m_RenameListBuffer.length() < 20)
                    m_RenameListBuffer += (char)c;
            }
            if (IsKeyPressed(KEY_BACKSPACE) && !m_RenameListBuffer.empty()) {
                m_RenameListBuffer.pop_back();
            }

            if (IsKeyPressed(KEY_ENTER))
            {
                if (!m_RenameListBuffer.empty()) {
                    m_Lists[i].Name = m_RenameListBuffer;
                    m_ListManager->updateList(m_Lists[i]);
                }
                m_RenamingListIndex = -1;
            }
            else if (IsKeyPressed(KEY_ESCAPE)) {
                m_RenamingListIndex = -1;
            }
        }
        else
        {
            float fontSize = listTabHeight * 0.4f;
            std::string displayName = m_Lists[i].Name;
            if (displayName.length() > 12) displayName = displayName.substr(0, 10) + "..";

            DrawLabel(displayName.c_str(), listRect.x + 15, listRect.y + (listRect.height - fontSize)/2, (int)fontSize, textColor);

            if (CheckCollisionPointRec(GetMousePosition(), listRect))
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    m_SelectedListIndex = i;
                    m_SelectedTaskIndex = -1;
                }
                if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
                {
                    m_RenamingListIndex = i;
                    m_RenameListBuffer = m_Lists[i].Name;
                }
            }
        }
        currentX += listTabWidth + listSpacing;
    }
    EndScissorMode();

    float ctrlBtnSize = 35.0f;
    float ctrlY = container.y + (container.height - ctrlBtnSize) / 2;

    Rectangle addBtn = { fixedControlsArea.x + 5, ctrlY, ctrlBtnSize, ctrlBtnSize };
    bool hoverAdd = CheckCollisionPointRec(GetMousePosition(), addBtn);
    DrawRectangleRounded(addBtn, 0.3f, 6, hoverAdd ? LIGHTGRAY : Fade(LIGHTGRAY, 0.5f));
    DrawText("+", addBtn.x + 11, addBtn.y + 8, 20, DARKGRAY);

    if (hoverAdd && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        TaskListSpecification newSpec;
        newSpec.UserId = AppResources::GetUserId();
        newSpec.Name = "List " + std::to_string(m_Lists.size() + 1);
        m_ListManager->createList(newSpec);
        ReloadLists();
        m_SelectedListIndex = m_Lists.size() - 1;
        if (totalContentWidth > scrollArea.width) listScrollX = totalContentWidth - scrollArea.width;
    }

    Rectangle delBtn = { fixedControlsArea.x + 10 + ctrlBtnSize, ctrlY, ctrlBtnSize, ctrlBtnSize };
    bool canDelete = !m_Lists.empty() && m_SelectedListIndex != -1;
    bool hoverDel = canDelete && CheckCollisionPointRec(GetMousePosition(), delBtn);
    DrawRectangleRounded(delBtn, 0.3f, 6, hoverDel ? Color{255, 200, 200, 255} : Fade(LIGHTGRAY, 0.3f));
    DrawText("-", delBtn.x + 13, delBtn.y + 8, 20, canDelete ? RED : Fade(GRAY, 0.3f));

    if (hoverDel && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        int idToDelete = m_Lists[m_SelectedListIndex].Id;
        m_ListManager->deleteList(idToDelete);
        ReloadLists();
        if (m_SelectedListIndex >= m_Lists.size()) {
            m_SelectedListIndex = std::max(0, static_cast<int>(m_Lists.size()) - 1);
        }
        if (m_Lists.empty()) m_SelectedListIndex = -1;
        m_SelectedTaskIndex = -1;
    }

    const char* tabs[2] = { "New", "Delete" };
    float buttonsStartY = container.y + (container.height - actionBtnHeight) * 0.5f;
    float buttonsStartX = container.x + container.width - rightSidePadding - (2 * actionBtnWidth) - (1 * btnSpacing);

    for (int i = 0; i < 2; i++)
    {
        Rectangle btn = {
            buttonsStartX + i * (actionBtnWidth + btnSpacing),
            buttonsStartY,
            actionBtnWidth,
            actionBtnHeight
        };

        bool isHover = CheckCollisionPointRec(GetMousePosition(), btn);
        DrawRectangleRounded(btn, 0.4f, 8, isHover ? LIGHTGRAY : RAYWHITE);

        float fontSize = actionBtnHeight * 0.5f;
        int txtW = MeasureText(tabs[i], (int)fontSize);
        DrawLabel(tabs[i], btn.x + (btn.width - txtW) / 2, btn.y + (actionBtnHeight - fontSize) * 0.45f, (int)fontSize, BLACK);

        if (isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (m_SelectedListIndex == -1) continue;
            int currentListID = m_Lists[m_SelectedListIndex].Id;

            if (i == 0) // New
            {
                TaskSpecification newTask;
                newTask.ListId = currentListID;
                newTask.Description = "";
                newTask.Status = false;
                auto newId = m_TaskManager->createTask(newTask);
                if (newId.has_value()) {
                    m_SelectedTaskIndex = newId.value();
                    LoadTaskToBuffer(m_SelectedTaskIndex);
                }
            }
            else if (i == 1) // Delete
            {
                if (m_SelectedTaskIndex != -1) {
                    m_TaskManager->deleteTask(m_SelectedTaskIndex);
                    m_SelectedTaskIndex = -1;
                    m_EditTitleBuffer = "";
                    m_EditContentBuffer = "";
                }
            }
        }
    }
}

void AppLayer::RenderContentArea(Rectangle bounds)
{
    DrawRectangleRec(bounds, Theme::BG_Main);

    switch (m_CurrentScreen)
    {
    case AppScreen::Tasks:
        RenderTasksScreen(bounds);
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

std::string WrapText(const std::string& text, int fontSize, float maxWidth)
{
    std::string finalOutput;
    std::stringstream ss(text);
    std::string rawLine;

    // Procesa cada línea SEPARADA por ENTER del usuario
    while (std::getline(ss, rawLine, '\n'))
    {
        std::string wrappedLine;
        std::string word;
        std::string line;

        for (char c : rawLine)
        {
            if (c == ' ')
            {
                std::string test = line + word + " ";
                if (MeasureText(test.c_str(), fontSize) > maxWidth)
                {
                    wrappedLine += line + "\n";
                    line = word + " ";
                }
                else
                {
                    line = test;
                }
                word.clear();
            }
            else
            {
                word += c;

                // Si la palabra sola excede el límite -> forzar salto
                if (MeasureText(word.c_str(), fontSize) > maxWidth)
                {
                    if (!line.empty()) wrappedLine += line + "\n";
                    wrappedLine += word + "\n";
                    word.clear();
                    line.clear();
                }
            }
        }

        // Empujar último fragmento
        if (!word.empty())
        {
            if (MeasureText((line + word).c_str(), fontSize) > maxWidth)
            {
                wrappedLine += line + "\n" + word;
            }
            else
            {
                wrappedLine += line + word;
            }
        }
        else
        {
            wrappedLine += line;
        }

        // Agregar salto original del usuario
        finalOutput += wrappedLine + "\n";
    }

    // Quitar un último salto extra
    if (!finalOutput.empty() && finalOutput.back() == '\n')
        finalOutput.pop_back();

    return finalOutput;
}

void AppLayer::RenderTasksScreen(Rectangle bounds)
{
    if (m_SelectedListIndex == -1 || m_SelectedListIndex >= m_Lists.size()) return;
    int currentListID = m_Lists[m_SelectedListIndex].Id;

    float padding = 20.0f;
    float panelWidth = bounds.width * 0.40f;

    static int titleCursor = 0;
    static int descCursor = 0;
    static bool titleSelectAll = false;
    static bool hasFocusedTitleOnce = false;

    // Timer estático para manejar la repetición de teclas
    static float inputRepeatTimer = 0.0f;

    float barHeight = 50.0f;
    float barSpacing = 15.0f;
    static char searchText[128] = "";
    static bool searchEdit = false;

    Rectangle barRect = {
        bounds.x + padding + 40,
        bounds.y + padding * 6.0f - barHeight - barSpacing,
        panelWidth - padding,
        barHeight
    };

    DrawRectangleRounded(barRect, 0.5f, 10, Theme::BG_Sidebar);
    GuiLabel({barRect.x + 15, barRect.y, 30, barHeight}, "#42#");

    Rectangle inputRect = { barRect.x + 45, barRect.y + 10, barRect.width - 80, barHeight - 20 };
    GuiSetStyle(TEXTBOX, BASE_COLOR_NORMAL, 0x00000000);
    GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, 0xffffffff);
    GuiSetStyle(TEXTBOX, BORDER_COLOR_NORMAL, 0x00000000);
    GuiSetStyle(TEXTBOX, TEXT_PADDING, 0);
    GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);

    if (GuiTextBox(inputRect, searchText, sizeof(searchText), searchEdit)) searchEdit = !searchEdit;
    if (!searchEdit && searchText[0] == '\0') DrawTextEx(AppResources::GetFont(), "Search tasks...", { inputRect.x + 2, inputRect.y + 2 }, 20, 0, Fade(WHITE, 0.3f));

    if (searchText[0] != '\0') {
        Rectangle clearBtn = { barRect.x + barRect.width - 35, barRect.y + 10, 30, 30 };
        int prevBase = GuiGetStyle(BUTTON, BASE_COLOR_NORMAL);
        int prevBorder = GuiGetStyle(BUTTON, BORDER_COLOR_NORMAL);
        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0x00000000);
        GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, 0x00000000);
        if (GuiButton(clearBtn, "#113#")) searchText[0] = '\0';
        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, prevBase);
        GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, prevBorder);
    }

    Rectangle panelBounds = {
        bounds.x + padding + 40,
        bounds.y + padding * 7.0f,
        panelWidth - padding,
        bounds.height - padding * 9.0f
    };

    TaskList tasks(m_Lists[m_SelectedListIndex], m_TaskManager.get());
    std::vector<TaskSpecification> displayItems;
    std::string searchStr = searchText;

    if (searchStr.empty()) {
        const auto& allTasks = tasks.getList();
        displayItems.reserve(allTasks.size());
        for (const auto& t : allTasks) {
            TaskSpecification spec;
            spec.Id = t.getId();
            spec.Title = t.getTitle();
            spec.Status = t.getStatus();
            spec.Description = t.getDescription();
            spec.DueDate = t.getDueDate();
            spec.CreateDate = t.getCreateDate();
            spec.ListId = currentListID;
            spec.isDeleted = t.getIsDeleted();
            displayItems.push_back(spec);
        }
    } else {
        displayItems = tasks.search(searchStr);
    }

    float btnHeight = 70.0f;
    float btnSpacing = 15.0f;
    float contentHeight = displayItems.size() * (btnHeight + btnSpacing) + 50;
    static float scrollY = 0;
    static bool dragging = false;
    static float dragOffset = 0;
    bool contentOverflows = contentHeight > panelBounds.height;

    if (contentOverflows) {
        float scrollbarWidth = 15.0f;
        Rectangle scrollBar = { panelBounds.x - scrollbarWidth - 5, panelBounds.y, scrollbarWidth, panelBounds.height };
        float handleHeight = panelBounds.height * (panelBounds.height / contentHeight);
        if (handleHeight < 20) handleHeight = 20;
        float scrollableArea = panelBounds.height - handleHeight;
        float handleY = panelBounds.y + (scrollY / (contentHeight - panelBounds.height)) * scrollableArea;
        Rectangle handle = { scrollBar.x, handleY, scrollBar.width, handleHeight };
        DrawRectangleRounded(handle, 20.0f, 10, BLACK);
        Vector2 mouse = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, handle)) { dragging = true; dragOffset = mouse.y - handle.y; }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) dragging = false;
        if (dragging) {
            float newHandleY = mouse.y - dragOffset;
            float ratio = (newHandleY - panelBounds.y) / scrollableArea;
            scrollY = ratio * (contentHeight - panelBounds.height);
        }
        if (CheckCollisionPointRec(mouse, panelBounds)) {
            float wheel = GetMouseWheelMove();
            if (wheel != 0) scrollY -= wheel * 30;
        }
        if (scrollY < 0) scrollY = 0;
        if (scrollY > contentHeight - panelBounds.height) scrollY = contentHeight - panelBounds.height;
    } else { scrollY = 0; dragging = false; }

    DrawRectangle(panelBounds.x, panelBounds.y, panelBounds.width, panelBounds.height, Fade(BLANK, 0));
    BeginScissorMode(panelBounds.x, panelBounds.y, panelBounds.width, panelBounds.height);
    float y = panelBounds.y - scrollY;
    Vector2 mouse = GetMousePosition();

    for (const auto& item : displayItems)
    {
        Rectangle btn = { panelBounds.x + 10, y, panelBounds.width - 20, btnHeight };
        bool isSelected = (item.Id == m_SelectedTaskIndex);
        Color btnColor = isSelected ? Theme::Selection_Blue : Theme::BG_Panel;
        Color txtColor = isSelected ? WHITE : Theme::Text_Dark;

        DrawRectangleRounded(btn, 0.2f, 12, btnColor);

        float checkSize = 30.0f;
        float checkMargin = 20.0f;
        Rectangle checkRect = { btn.x + btn.width - checkSize - checkMargin, btn.y + (btnHeight - checkSize) / 2, checkSize, checkSize };
        DrawRectangleRounded(checkRect, 0.3f, 6, item.Status ? Theme::Border_Panel : Fade(GRAY, 0.5f));
        DrawRectangleRoundedLines(checkRect, 0.3f, 6, item.Status ? WHITE : GRAY);
        if (item.Status) DrawLabel("v", checkRect.x + 8, checkRect.y + 2, 25, WHITE);

        float textStartX = btn.x + 30;
        float maxTextWidth = (checkRect.x - 10) - textStartX;
        std::string displayTitle = item.Title.empty() ? "Untitled Task" : item.Title;
        if (MeasureText(displayTitle.c_str(), 30) > maxTextWidth) {
            while (displayTitle.length() > 3 && MeasureText((displayTitle + "...").c_str(), 30) > maxTextWidth) displayTitle.pop_back();
            displayTitle += "...";
        }
        DrawLabel(displayTitle.c_str(), textStartX, btn.y + (btnHeight - 30) / 2, 30, txtColor);

        if (CheckCollisionPointRec(mouse, checkRect)) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                TaskSpecification u = item; u.Status = !item.Status;
                m_TaskManager->updateTask(u);
            }
        } else if (CheckCollisionPointRec(mouse, btn)) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                m_SelectedTaskIndex = item.Id;
                LoadTaskToBuffer(m_SelectedTaskIndex);
                hasFocusedTitleOnce = false;
                titleSelectAll = false;
                titleCursor = 0;
                descCursor = 0;
            }
        }
        y += btnHeight + btnSpacing;
    }
    EndScissorMode();

    auto AutoSave = [this, currentListID]() {
        if (m_SelectedTaskIndex == -1) return;
        auto currentTasks = m_TaskManager->loadTasks(currentListID);
        auto it = std::find_if(currentTasks.begin(), currentTasks.end(),
            [this](const Task& t) { return t.getId() == m_SelectedTaskIndex; });

        if (it != currentTasks.end())
        {
            TaskSpecification spec;
            spec.Id = it->getId();
            spec.ListId = currentListID;
            spec.Status = it->getStatus();
            spec.CreateDate = it->getCreateDate();
            spec.isDeleted = it->getIsDeleted();
            spec.Title = m_EditTitleBuffer;
            spec.Description = m_EditContentBuffer;
            if (m_DayBuffer.empty() || m_MonthBuffer.empty() || m_YearBuffer.empty()) {
                spec.DueDate = std::chrono::system_clock::from_time_t(0);
            } else {
                std::string d = (m_DayBuffer.length() == 1) ? "0" + m_DayBuffer : m_DayBuffer;
                std::string m = (m_MonthBuffer.length() == 1) ? "0" + m_MonthBuffer : m_MonthBuffer;
                std::string y = m_YearBuffer;
                std::string dateStr = y + "-" + m + "-" + d + " 12:00:00";
                spec.DueDate = TaskUtilities::stringToTimePoint(dateStr);
            }
            m_TaskManager->updateTask(spec);
        }
    };

    // Lambda Helper para repetición de teclas (Flechas, Backspace)
    auto IsKeyActive = [&](int key) -> bool {
        if (IsKeyPressed(key)) {
            inputRepeatTimer = 0.4f; // Delay inicial (ms)
            return true;
        }
        if (IsKeyDown(key)) {
            inputRepeatTimer -= GetFrameTime();
            if (inputRepeatTimer <= 0.0f) {
                inputRepeatTimer = 0.03f; // Velocidad de repetición
                return true;
            }
        }
        return false;
    };

    float viewWidth = bounds.width * 0.5f;
    Rectangle viewer = { bounds.x + bounds.width * 0.5f + 20, bounds.y + padding, bounds.width * 0.45f - 20, bounds.height - padding * 2 };
    float innerScale = 0.95f;
    Rectangle inner = { viewer.x + viewer.width * (1.0f - innerScale) / 2, viewer.y + 27 + viewer.height * (1.0f - innerScale) / 2, viewer.width * innerScale, viewer.height * innerScale - 27 };

    DrawRectangleRounded(viewer, 0.05f, 20, Theme::Border_Panel);
    DrawRectangleRounded(inner, 0.05f, 20, Theme::BG_Panel);

    static int lastSelectedIndex = -1;
    if (m_SelectedTaskIndex != lastSelectedIndex) {
        if (m_SelectedTaskIndex != -1) LoadTaskToBuffer(m_SelectedTaskIndex);
        else {
            m_EditTitleBuffer.clear(); m_EditContentBuffer.clear();
            m_DayBuffer.clear(); m_MonthBuffer.clear(); m_YearBuffer.clear();
        }
        lastSelectedIndex = m_SelectedTaskIndex;
        hasFocusedTitleOnce = false;
        titleSelectAll = false;
    }

    if (m_SelectedTaskIndex != -1)
    {
        static bool titleActive = false, contentActive = false;
        static bool dayActive = false, monthActive = false, yearActive = false;

        float paddingInner = 10.0f;
        Rectangle titleBox = { inner.x + paddingInner, inner.y + paddingInner + 10, inner.width - paddingInner*2, 50 };
        Rectangle contentBox = { inner.x + paddingInner, inner.y + 80, inner.width - paddingInner*2, inner.height - 150 };

        float bottomY = inner.y + inner.height - 45;
        float boxHeight = 35.0f;
        float dayWidth = 40.0f; float monthWidth = 40.0f; float yearWidth = 60.0f; float spacingD = 5.0f;
        Rectangle boxYear  = { inner.x + inner.width - yearWidth - 20, bottomY, yearWidth, boxHeight };
        Rectangle boxMonth = { boxYear.x - spacingD - monthWidth, bottomY, monthWidth, boxHeight };
        Rectangle boxDay   = { boxMonth.x - spacingD - dayWidth, bottomY, dayWidth, boxHeight };

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mp = GetMousePosition();
            bool prevTitleActive = titleActive;

            titleActive = CheckCollisionPointRec(mp, titleBox);
            contentActive = CheckCollisionPointRec(mp, contentBox);
            dayActive = CheckCollisionPointRec(mp, boxDay);
            monthActive = CheckCollisionPointRec(mp, boxMonth);
            yearActive = CheckCollisionPointRec(mp, boxYear);

            if (titleActive) {
                if (!prevTitleActive && !hasFocusedTitleOnce) {
                    titleSelectAll = true;
                    hasFocusedTitleOnce = true;
                    titleCursor = (int)m_EditTitleBuffer.length();
                } else {
                    titleSelectAll = false;
                    titleCursor = (int)m_EditTitleBuffer.length();
                }
            } else {
                titleSelectAll = false;
            }

            if (contentActive) {
                descCursor = (int)m_EditContentBuffer.length();
            }
        }

        bool changed = false;

        if (titleActive) {
            if (IsKeyActive(KEY_LEFT)) { titleCursor = std::max(0, titleCursor - 1); titleSelectAll = false; }
            if (IsKeyActive(KEY_RIGHT)) { titleCursor = std::min((int)m_EditTitleBuffer.length(), titleCursor + 1); titleSelectAll = false; }

            int c;
            while ((c = GetCharPressed()) > 0) {
                if (m_EditTitleBuffer.size() < 60) {
                    if (titleSelectAll) { m_EditTitleBuffer.clear(); titleCursor = 0; titleSelectAll = false; }
                    m_EditTitleBuffer.insert(titleCursor, 1, (char)c);
                    titleCursor++; changed = true;
                }
            }
            if (IsKeyActive(KEY_BACKSPACE)) {
                if (titleSelectAll) { m_EditTitleBuffer.clear(); titleCursor = 0; titleSelectAll = false; changed = true; }
                else if (titleCursor > 0 && !m_EditTitleBuffer.empty()) {
                    m_EditTitleBuffer.erase(titleCursor - 1, 1);
                    titleCursor--; changed = true;
                }
            }
        }
        else if (contentActive) {
            if (IsKeyActive(KEY_LEFT)) descCursor = std::max(0, descCursor - 1);
            if (IsKeyActive(KEY_RIGHT)) descCursor = std::min((int)m_EditContentBuffer.length(), descCursor + 1);

            int c;
            while ((c = GetCharPressed()) > 0) {
                m_EditContentBuffer.insert(descCursor, 1, (char)c);
                descCursor++; changed = true;
            }
            if (IsKeyActive(KEY_BACKSPACE)) {
                if (descCursor > 0 && !m_EditContentBuffer.empty()) {
                    m_EditContentBuffer.erase(descCursor - 1, 1);
                    descCursor--; changed = true;
                }
            }
            if (IsKeyPressed(KEY_ENTER)) {
                m_EditContentBuffer.insert(descCursor, 1, '\n');
                descCursor++; changed = true;
            }
        }
        else {
            int c;
            while ((c = GetCharPressed()) > 0) {
                if (isdigit(c)) {
                    if (dayActive && m_DayBuffer.size() < 2) { m_DayBuffer += (char)c; changed = true; }
                    else if (monthActive && m_MonthBuffer.size() < 2) { m_MonthBuffer += (char)c; changed = true; }
                    else if (yearActive && m_YearBuffer.size() < 4) { m_YearBuffer += (char)c; changed = true; }
                }
            }
            if (IsKeyActive(KEY_BACKSPACE)) {
                 if (dayActive && !m_DayBuffer.empty()) { m_DayBuffer.pop_back(); changed = true; }
                 if (monthActive && !m_MonthBuffer.empty()) { m_MonthBuffer.pop_back(); changed = true; }
                 if (yearActive && !m_YearBuffer.empty()) { m_YearBuffer.pop_back(); changed = true; }
            }
        }

        if (changed) AutoSave();

        if (m_EditTitleBuffer.empty() && !titleActive) {
            DrawTextEx(AppResources::GetFont(), "Untitled Task", { titleBox.x + 5, titleBox.y + 8 }, 34, 0, Fade(GRAY, 0.4f));
        }

        BeginScissorMode((int)titleBox.x, (int)titleBox.y, (int)titleBox.width, (int)titleBox.height);
            if (titleActive && titleSelectAll && !m_EditTitleBuffer.empty()) {
                Vector2 textSize = MeasureTextEx(AppResources::GetFont(), m_EditTitleBuffer.c_str(), 34, 0);
                DrawRectangle(titleBox.x + 5, titleBox.y + 8, textSize.x, textSize.y, Fade(Theme::Selection_Blue, 0.5f));
            }
            DrawTextEx(AppResources::GetFont(), m_EditTitleBuffer.c_str(), { titleBox.x + 5, titleBox.y + 8 }, 34, 0, Theme::Text_Dark);

            if (titleActive && !titleSelectAll) {
                std::string sub = m_EditTitleBuffer.substr(0, titleCursor);
                Vector2 subSize = MeasureTextEx(AppResources::GetFont(), sub.c_str(), 34, 0);
                if (((int)(GetTime() * 2.0f)) % 2 == 0) {
                    DrawRectangle(titleBox.x + 5 + subSize.x, titleBox.y + 8, 2, 34, Theme::Selection_Blue);
                }
            }
        EndScissorMode();
        if (titleActive) DrawRectangle(titleBox.x, titleBox.y + titleBox.height - 2, titleBox.width, 2, Theme::Selection_Blue);

        if (m_EditContentBuffer.empty() && !contentActive) {
            DrawTextEx(AppResources::GetFont(), "Add a description...", { contentBox.x + 5, contentBox.y + 5 }, 20, 0, Fade(GRAY, 0.4f));
        }

        BeginScissorMode((int)contentBox.x, (int)contentBox.y, (int)contentBox.width, (int)contentBox.height);
            float descX = contentBox.x + 5;
            float descY = contentBox.y + 5;
            float lineHeight = 20.0f;
            float maxWidth = contentBox.width - 10;
            if (descCursor > (int)m_EditContentBuffer.length()) descCursor = (int)m_EditContentBuffer.length();

            std::string currentLine;
            Vector2 visualCursorPos = { descX, descY };

            for (int i = 0; i < m_EditContentBuffer.length(); i++)
            {
                char c = m_EditContentBuffer[i];
                if (i == descCursor) {
                    visualCursorPos = { descX + MeasureTextEx(AppResources::GetFont(), currentLine.c_str(), 20, 0).x, descY };
                }
                if (c == '\n') {
                    DrawTextEx(AppResources::GetFont(), currentLine.c_str(), { descX, descY }, 20, 0, Theme::Text_Dark);
                    descY += lineHeight;
                    currentLine = "";
                    continue;
                }
                std::string temp = currentLine + c;
                if (MeasureTextEx(AppResources::GetFont(), temp.c_str(), 20, 0).x > maxWidth) {
                    DrawTextEx(AppResources::GetFont(), currentLine.c_str(), { descX, descY }, 20, 0, Theme::Text_Dark);
                    descY += lineHeight;
                    currentLine = "";
                    currentLine += c;
                } else {
                    currentLine += c;
                }
            }
            DrawTextEx(AppResources::GetFont(), currentLine.c_str(), { descX, descY }, 20, 0, Theme::Text_Dark);

            if (descCursor == m_EditContentBuffer.length()) {
                visualCursorPos = { descX + MeasureTextEx(AppResources::GetFont(), currentLine.c_str(), 20, 0).x, descY };
            }

            if (contentActive && ((int)(GetTime() * 2.0f)) % 2 == 0) {
                DrawRectangle(visualCursorPos.x, visualCursorPos.y, 2, 20, Theme::Selection_Blue);
            }
        EndScissorMode();

        DrawRectangleRounded(boxDay, 0.3f, 4, dayActive ? Theme::Selection_Blue : Fade(LIGHTGRAY, 0.3f));
        DrawRectangleRounded(boxMonth, 0.3f, 4, monthActive ? Theme::Selection_Blue : Fade(LIGHTGRAY, 0.3f));
        DrawRectangleRounded(boxYear, 0.3f, 4, yearActive ? Theme::Selection_Blue : Fade(LIGHTGRAY, 0.3f));

        Rectangle btnClear = { boxDay.x - 80, bottomY + 5, 25, 25 };
        bool hoverClear = CheckCollisionPointRec(GetMousePosition(), btnClear);
        DrawRectangleRounded(btnClear, 0.3f, 4, hoverClear ? Fade(RED, 0.2f) : Fade(GRAY, 0.2f));
        DrawRectangleRoundedLines(btnClear, 0.3f, 4, hoverClear ? RED : GRAY);
        DrawText("x", btnClear.x + 8, btnClear.y + 4, 20, hoverClear ? RED : GRAY);
        if (hoverClear && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            m_DayBuffer = ""; m_MonthBuffer = ""; m_YearBuffer = "";
            AutoSave();
        }

        DrawLabel("Due:", boxDay.x - 45, bottomY + 8, 20, GRAY);
        DrawLabel("/", boxDay.x + dayWidth - 3, bottomY + 8, 20, GRAY);
        DrawLabel("/", boxMonth.x + monthWidth - 3, bottomY + 8, 20, GRAY);

        auto DrawCentered = [this](std::string& txt, Rectangle r, const char* ph, bool active) {
            Color txtColor = active ? WHITE : BLACK;
            if (txt.empty()) this->DrawLabel(ph, r.x + 5, r.y + 8, 20, Fade(GRAY, 0.5f));
            else {
                int w = MeasureText(txt.c_str(), 20);
                this->DrawLabel(txt.c_str(), r.x + (r.width - w)/2, r.y + 8, 20, txtColor);
            }
        };
        DrawCentered(m_DayBuffer, boxDay, "DD", dayActive);
        DrawCentered(m_MonthBuffer, boxMonth, "MM", monthActive);
        DrawCentered(m_YearBuffer, boxYear, "YYYY", yearActive);

    } else {
        const char* msg = "Pick a Task";
        int w = MeasureText(msg, 20);
        DrawText(msg, inner.x + (inner.width - w)/2, inner.y + inner.height/2, 20, LIGHTGRAY);
    }
}

void AppLayer::RenderCalendarScreen(Rectangle bounds)
{
    if (m_CalYear == 0 || m_CalYear == 2025 && m_CalMonth == 0 && m_SelectedDay == -1)
    {
        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);
        m_CalYear = now->tm_year + 1900;
        m_CalMonth = now->tm_mon;

        m_SelectedDay = now->tm_mday;
        m_SelectedMonth = m_CalMonth;
        m_SelectedYear = m_CalYear;
    }

    float gridWidth = bounds.width * 0.65f;
    float padding = 20.0f;

    Rectangle gridRect = {
        bounds.x + padding,
        bounds.y + padding,
        gridWidth - (padding * 2),
        bounds.height - (padding * 2)
    };

    Rectangle listRect = {
        bounds.x + gridWidth,
        bounds.y + padding,
        bounds.width - gridWidth - padding,
        bounds.height - (padding * 2)
    };

    // Fondo general
    DrawRectangleRec(bounds, Theme::BG_Main);

    // Dibujar las dos secciones
    DrawCalendarGrid(gridRect);
    DrawDayList(listRect);
}

void AppLayer::RenderSettingsScreen(Rectangle bounds)
{
    const float margin = 40.0f;
    Rectangle panelRect = {
        bounds.x + margin,
        bounds.y + margin,
        bounds.width - (margin * 2),
        bounds.height - (margin * 2)
    };

    DrawRectangleRounded(panelRect, 0.02f, 10, Theme::BG_Panel);

    if (m_SettingsLogo.id != 0)
    {
        float targetHeight = panelRect.height * 0.85f;

        float scale = targetHeight / (float)m_SettingsLogo.height;

        float logoW = (float)m_SettingsLogo.width * scale;
        float logoH = (float)m_SettingsLogo.height * scale;

        float logoX = panelRect.x + panelRect.width - logoW - 50.0f;

        float logoY = panelRect.y + (panelRect.height - logoH) / 2.0f;

        if (logoX > (panelRect.x + 550.0f))
        {
            DrawTextureEx(m_SettingsLogo, {logoX, logoY}, 0.0f, scale, WHITE);
        }
    }

    // --- CONTENIDO (IZQUIERDA) ---
    int defaultSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    auto DrawStyledButton = [](Rectangle r, const char* text, Color baseColor) -> bool
    {
        int prevBase = GuiGetStyle(BUTTON, BASE_COLOR_NORMAL);
        int prevText = GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL);
        int prevBorder = GuiGetStyle(BUTTON, BORDER_COLOR_NORMAL);
        int prevAlign = GuiGetStyle(BUTTON, TEXT_ALIGNMENT);

        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(baseColor));
        GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
        GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(baseColor));
        GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

        bool clicked = GuiButton(r, text);

        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, prevBase);
        GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, prevText);
        GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, prevBorder);
        GuiSetStyle(BUTTON, TEXT_ALIGNMENT, prevAlign);

        return clicked;
    };

    float startX = panelRect.x + 60.0f;
    float cursorY = panelRect.y + 50.0f;
    float contentWidth = panelRect.width - 120.0f;
    if (contentWidth > 500.0f) contentWidth = 500.0f;

    DrawLabel("Settings", (int)startX, (int)cursorY, 50, Theme::Text_Dark);
    DrawRectangle((int)startX, (int)cursorY + 60, (int)contentWidth, 3, Theme::BG_Sidebar);
    cursorY += 90.0f;

    DrawLabel("Account", (int)startX, (int)cursorY, 24, Fade(Theme::Text_Dark, 0.5f));
    cursorY += 40.0f;

    std::string username = AppResources::GetUsername();

    GuiLabel({startX, cursorY, contentWidth, 30},
        GuiIconText(ICON_PLAYER, TextFormat("  Logged in as: %s", username.c_str()))
    );

    cursorY += 40.0f;

    if (DrawStyledButton({startX, cursorY, contentWidth, 45}, GuiIconText(ICON_EXIT, "Log Out"), Theme::BG_Sidebar))
    {
        AppResources::ClearSession();
        TransitionTo<AuthLayer>();
    }
    cursorY += 80.0f;

    DrawLabel("About & Data", (int)startX, (int)cursorY, 24, Fade(Theme::Text_Dark, 0.5f));
    cursorY += 40.0f;

    if (DrawStyledButton({startX, cursorY, contentWidth, 45}, GuiIconText(211, "View Source Code"), Theme::BG_Sidebar))
    {
        OpenURL("https://github.com/NuclearGecko74/todo-list-cpp");
    }

    cursorY += 55.0f;

    if (DrawStyledButton({startX, cursorY, contentWidth, 45}, GuiIconText(ICON_BIN, "Clear Completed Tasks"), GRAY))
    {
        for (const auto& list : m_Lists)
        {
            auto tasks = m_TaskManager->loadTasks(list.Id);

            for (const auto& task : tasks)
            {
                if (task.getStatus())
                {
                    m_TaskManager->deleteTask(task.getId());
                }
            }
        }
        m_SelectedTaskIndex = -1;

        m_EditTitleBuffer.clear();
        m_EditContentBuffer.clear();
        m_DayBuffer.clear();
        m_MonthBuffer.clear();
        m_YearBuffer.clear();
    }

    float footerY = panelRect.y + panelRect.height - 40.0f;
    DrawLabel("RizzList v1.0.0", (int)startX, (int)footerY, 16, Fade(Theme::Text_Dark, 0.4f));

    GuiSetStyle(DEFAULT, TEXT_SIZE, defaultSize);
    GuiSetStyle(TOGGLE, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
}

int AppLayer::GetDaysInMonth(int month, int year)
{
    if (month == 1)
    {
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
            return 29;
        return 28;
    }
    if (month == 3 || month == 5 || month == 8 || month == 10) return 30;
    return 31;
}

int AppLayer::GetDayOfWeek(int d, int m, int y)
{
    std::tm timeIn = { 0 };
    timeIn.tm_year = y - 1900;
    timeIn.tm_mon = m;
    timeIn.tm_mday = d;
    std::mktime(&timeIn);
    return timeIn.tm_wday;
}

bool AppLayer::IsSameDate(const std::chrono::system_clock::time_point& a, int d, int m, int y)
{
    auto tt = std::chrono::system_clock::to_time_t(a);
    std::tm tm = *std::localtime(&tt);
    return (tm.tm_year + 1900 == y && tm.tm_mon == m && tm.tm_mday == d);
}

void AppLayer::DrawCalendarGrid(Rectangle bounds)
{
    DrawRectangleRounded(bounds, 0.02f, 10, Theme::BG_Panel);

    // --- HEADER (MES Y AÑO + BOTONES) ---
    float headerHeight = 60.0f;
    float startX = bounds.x + 20.0f;
    float startY = bounds.y + 20.0f;

    // Nombres de meses
    const char* months[] = { "January", "February", "March", "April", "May", "June",
                             "July", "August", "September", "October", "November", "December" };

    // Título
    DrawTextEx(AppResources::GetFont(), TextFormat("%s %d", months[m_CalMonth], m_CalYear),
               { startX, startY }, 40, 0, Theme::Text_Dark);

    // Botones Prev/Next (< >)
    float btnSize = 30.0f;
    if (GuiButton({ bounds.x + bounds.width - 80, startY + 5, btnSize, btnSize }, "#118#")) // <
    {
        m_CalMonth--;
        if (m_CalMonth < 0) { m_CalMonth = 11; m_CalYear--; }
    }

    if (GuiButton({ bounds.x + bounds.width - 40, startY + 5, btnSize, btnSize }, "#119#")) // >
    {
        m_CalMonth++;
        if (m_CalMonth > 11) { m_CalMonth = 0; m_CalYear++; }
    }

    // --- GRID DE DÍAS ---
    float gridTop = startY + 60.0f;
    float cellWidth = (bounds.width - 40) / 7.0f;
    float cellHeight = (bounds.height - 100) / 6.0f; // 6 semanas max

    // Dibujar Nombres de Días (Sun, Mon...)
    const char* days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    for (int i = 0; i < 7; i++)
    {
        DrawTextEx(AppResources::GetFont(), days[i],
            { bounds.x + 20 + (i * cellWidth) + (cellWidth/2 - 15), gridTop },
            20, 0, GRAY);
    }

    gridTop += 30.0f; // Bajar para los números

    // Lógica del Grid
    int daysInMonth = GetDaysInMonth(m_CalMonth, m_CalYear);
    int startDayOfWeek = GetDayOfWeek(1, m_CalMonth, m_CalYear); // 0..6

    int currentDay = 1;

    // Iterar semanas (filas) y días (columnas)
    for (int row = 0; row < 6; row++)
    {
        for (int col = 0; col < 7; col++)
        {
            // Espacios vacíos antes del día 1
            if (row == 0 && col < startDayOfWeek) continue;

            // Si ya pasamos el último día
            if (currentDay > daysInMonth) break;

            // Coordenadas de la celda
            Rectangle cell = {
                bounds.x + 20 + (col * cellWidth),
                gridTop + (row * cellHeight),
                cellWidth - 5,
                cellHeight - 5
            };

            // ESTADOS VISUALES
            bool isSelected = (currentDay == m_SelectedDay && m_CalMonth == m_SelectedMonth && m_CalYear == m_SelectedYear);
            bool isHover = CheckCollisionPointRec(GetMousePosition(), cell);

            // Fondo Celda
            Color cellColor = Theme::BG_Panel; // Blanco por defecto
            if (isSelected) cellColor = Theme::Selection_Blue; // Azul si seleccionado
            else if (isHover) cellColor = Fade(LIGHTGRAY, 0.3f); // Gris si hover

            DrawRectangleRounded(cell, 0.2f, 6, cellColor);

            // Número del día
            Color textColor = isSelected ? WHITE : Theme::Text_Dark;
            DrawTextEx(AppResources::GetFont(), TextFormat("%d", currentDay),
                       { cell.x + 10, cell.y + 10 }, 24, 0, textColor);

            // --- INDICADOR DE TAREAS (El puntito) ---
            // Buscamos si hay tareas para este día en TODAS las listas
            // (Esto no es ultra eficiente, pero para <1000 tareas es instantáneo)
            bool hasTask = false;
            for (const auto& list : m_Lists)
            {
                auto tasks = m_TaskManager->loadTasks(list.Id);
                for (const auto& t : tasks) {
                    // Si tiene fecha válida Y coincide con este día
                    if (IsSameDate(t.getDueDate(), currentDay, m_CalMonth, m_CalYear) && !t.getStatus()) {
                        hasTask = true;
                        break;
                    }
                }
                if (hasTask) break;
            }

            if (hasTask)
            {
                // Dibujar puntito morado
                DrawCircle(cell.x + cell.width/2, cell.y + cell.height - 15, 4, isSelected ? WHITE : Theme::BG_Sidebar);
            }

            // CLICK
            if (isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                m_SelectedDay = currentDay;
                m_SelectedMonth = m_CalMonth;
                m_SelectedYear = m_CalYear;
            }

            currentDay++;
        }
    }
}

void AppLayer::DrawDayList(Rectangle bounds)
{
    // Fondo Panel Derecho
    DrawRectangleRounded(bounds, 0.02f, 10, Theme::BG_Panel);

    float padding = 20.0f;
    float startX = bounds.x + padding;
    float currentY = bounds.y + padding;

    // Título: "Tasks for Oct 12"
    if (m_SelectedDay != -1)
    {
        const char* months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
        DrawTextEx(AppResources::GetFont(), TextFormat("Tasks for %s %d", months[m_SelectedMonth], m_SelectedDay),
                   { startX, currentY }, 30, 0, Theme::Text_Dark);

        currentY += 50.0f;

        // --- CARGAR TAREAS DEL DÍA ---
        // Recorremos todas las listas y buscamos tareas de ese día
        // (En una app real, guardarías esto en m_DayTasks para no hacerlo cada frame)
        std::vector<TaskSpecification> tasksForDay;

        for (const auto& list : m_Lists)
        {
            auto tasks = m_TaskManager->loadTasks(list.Id);
            for (const auto& t : tasks) {
                if (IsSameDate(t.getDueDate(), m_SelectedDay, m_SelectedMonth, m_SelectedYear)) {
                    TaskSpecification spec;
                    spec.Id = t.getId();
                    spec.Title = t.getTitle();
                    spec.Status = t.getStatus();
                    tasksForDay.push_back(spec);
                }
            }
        }

        // --- LISTAR TAREAS ---
        if (tasksForDay.empty())
        {
            DrawTextEx(AppResources::GetFont(), "No tasks due this day.", { startX, currentY }, 20, 0, GRAY);
            DrawTextEx(AppResources::GetFont(), "Chill time!", { startX, currentY + 30 }, 20, 0, Fade(Theme::Selection_Blue, 0.8f));
        }
        else
        {
            // Usar Scissor por si son muchas
            BeginScissorMode(bounds.x, currentY, bounds.width, bounds.height - 60);

            for (const auto& task : tasksForDay)
            {
                // Tarjeta simple
                Rectangle taskRect = { startX, currentY, bounds.width - (padding*2), 50 };

                DrawRectangleRounded(taskRect, 0.3f, 6, Theme::BG_Sidebar); // Fondo Oscuro

                // Texto
                DrawTextEx(AppResources::GetFont(), task.Title.c_str(), { taskRect.x + 15, taskRect.y + 15 }, 20, 0, WHITE);

                // Status (Check simple)
                if (task.Status)
                    DrawTextEx(AppResources::GetFont(), "Done", { taskRect.x + taskRect.width - 60, taskRect.y + 15 }, 18, 0, GREEN);

                currentY += 60.0f;
            }

            EndScissorMode();
        }
    }
    else
    {
        DrawTextEx(AppResources::GetFont(), "Select a day", { startX, currentY }, 30, 0, GRAY);
    }
}