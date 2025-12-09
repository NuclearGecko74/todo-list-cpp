#include "AppLayer.h"

#include <sstream>

#include "AuthLayer.h"
#include "Core/Application.h"

#include "raylib.h"
#include "raygui.h"
#include "UI/Theme.h"
#include "Model/Utilities.h"

AppLayer::AppLayer()
{
    m_ListManager= std::make_unique<ListManager>(AppResources::GetDatabase());
    m_TaskManager = std::make_unique<TaskManager>(AppResources::GetDatabase());
    ReloadLists();
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
    // 1. DIBUJAR FONDO DEL CONTENEDOR
    Rectangle container = {
        bounds.x + PADDING * 2,
        bounds.y + PADDING,
        bounds.width - (PADDING * 6),
        bounds.height - (PADDING * 2)
    };
    DrawRectangleRounded(container, 0.5f, 10, Theme::Border_Panel);

    if (m_CurrentScreen != AppScreen::Tasks) return;

    // =================================================================================
    // CÁLCULOS DE DISEÑO (LAYOUT)
    // =================================================================================

    // --- LADO DERECHO (Botones de Tarea: New, Save, Delete) ---
    float actionBtnWidth  = container.width * 0.10f;
    float actionBtnHeight = container.height * 0.65f;
    float spacing         = container.width * 0.01f;
    float margin          = container.width * 0.0001f;

    // Ancho total ocupado por los botones de la derecha + margen
    float rightSideWidth = (3 * actionBtnWidth) + (3 * spacing) + margin + 50;

    // --- LADO IZQUIERDO (Listas + Controles Fijos) ---
    float fixedControlsWidth = 90.0f; // Espacio para [ + ] y [ - ]

    // Área donde ocurre el SCROLL de las listas
    Rectangle scrollArea = {
        container.x + 10,
        container.y,
        container.width - rightSideWidth - fixedControlsWidth,
        container.height
    };

    // Área FIJA para los botones [+] y [-] (Justo después del scroll)
    Rectangle fixedControlsArea = {
        scrollArea.x + scrollArea.width,
        container.y,
        fixedControlsWidth,
        container.height
    };

    // =================================================================================
    // ZONA 1: LISTAS (CON SCROLL Y RENOMBRADO)
    // =================================================================================

    static float listScrollX = 0.0f;
    float listTabWidth = 150.0f;
    float listTabHeight = actionBtnHeight;
    float listSpacing = 10.0f;

    // Ancho total del contenido (solo las pestañas)
    float totalContentWidth = m_Lists.size() * (listTabWidth + listSpacing);

    // --- LÓGICA DE SCROLL ---
    // Solo permitir scroll si el mouse está SOBRE el área de listas
    if (CheckCollisionPointRec(GetMousePosition(), scrollArea))
    {
        float wheel = GetMouseWheelMove();
        if (wheel != 0)
        {
            listScrollX -= wheel * 30.0f;

            // Clamping
            float maxScroll = totalContentWidth - scrollArea.width;
            if (maxScroll < 0) maxScroll = 0;

            if (listScrollX < 0) listScrollX = 0;
            if (listScrollX > maxScroll) listScrollX = maxScroll;
        }
    }

    // --- INICIO DEL RECORTE (SCISSOR MODE) ---
    BeginScissorMode((int)scrollArea.x, (int)scrollArea.y, (int)scrollArea.width, (int)scrollArea.height);

    float currentX = scrollArea.x + 10 - listScrollX;
    float centerY = container.y + (container.height - listTabHeight) * 0.5f;

    for (int i = 0; i < m_Lists.size(); i++)
    {
        Rectangle listRect = { currentX, centerY, listTabWidth, listTabHeight };

        // Estilo visual
        bool isSelected = (i == m_SelectedListIndex);
        Color tabColor = isSelected ? Theme::Selection_Blue : RAYWHITE;
        Color textColor = isSelected ? WHITE : BLACK;

        DrawRectangleRounded(listRect, 0.4f, 8, tabColor);

        // ------------------------------------------------------------
        //               LÓGICA DE RENOMBRADO
        // ------------------------------------------------------------
        if (m_RenamingListIndex == i)
        {
            // MODO EDICIÓN: Dibujar caja de texto blanca
            Rectangle inputRect = { listRect.x + 5, listRect.y + 5, listRect.width - 10, listRect.height - 10 };
            DrawRectangleRec(inputRect, WHITE);
            DrawRectangleLinesEx(inputRect, 2, Theme::Selection_Blue);

            // Dibujar texto del buffer
            DrawLabel(m_RenameListBuffer.c_str(), inputRect.x + 5, inputRect.y + 8, 20, BLACK);

            // Capturar escritura
            int c;
            while ((c = GetCharPressed()) > 0) {
                // Solo caracteres imprimibles y longitud razonable
                if (c >= 32 && c <= 125 && m_RenameListBuffer.length() < 20)
                    m_RenameListBuffer += (char)c;
            }
            // Borrar
            if (IsKeyPressed(KEY_BACKSPACE) && !m_RenameListBuffer.empty()) {
                m_RenameListBuffer.pop_back();
            }

            // GUARDAR (Enter)
            if (IsKeyPressed(KEY_ENTER))
            {
                if (!m_RenameListBuffer.empty()) {
                    m_Lists[i].Name = m_RenameListBuffer;     // Actualizar memoria
                    m_ListManager->updateList(m_Lists[i]);    // Actualizar BD
                }
                m_RenamingListIndex = -1; // Salir de edición
            }
            // CANCELAR (Escape)
            else if (IsKeyPressed(KEY_ESCAPE)) {
                m_RenamingListIndex = -1;
            }
        }
        else
        {
            // MODO NORMAL: Dibujar etiqueta
            float fontSize = listTabHeight * 0.4f;
            // Truncar visualmente si es muy largo (opcional)
            std::string displayName = m_Lists[i].Name;
            if (displayName.length() > 12) displayName = displayName.substr(0, 10) + "..";

            DrawLabel(displayName.c_str(), listRect.x + 15, listRect.y + (listRect.height - fontSize)/2, (int)fontSize, textColor);

            // INTERACCIÓN MOUSE
            if (CheckCollisionPointRec(GetMousePosition(), listRect))
            {
                // Clic Izquierdo: Seleccionar
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    m_SelectedListIndex = i;
                    m_SelectedTaskIndex = -1;
                }

                // Clic Derecho: INICIAR RENOMBRADO
                if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
                {
                    m_RenamingListIndex = i;
                    m_RenameListBuffer = m_Lists[i].Name; // Cargar nombre actual
                }
            }
        }
        // ------------------------------------------------------------

        currentX += listTabWidth + listSpacing;
    }

    EndScissorMode(); // <--- FIN DEL RECORTE


    // =================================================================================
    // ZONA 2: CONTROLES FIJOS ([+] y [-])
    // =================================================================================

    float ctrlBtnSize = 35.0f;
    float ctrlY = container.y + (container.height - ctrlBtnSize) / 2;

    // --- BOTÓN [+] ADD ---
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

        m_SelectedListIndex = m_Lists.size() - 1; // Ir a la nueva
        // Scroll al final
        if (totalContentWidth > scrollArea.width) listScrollX = totalContentWidth - scrollArea.width;
    }

    // --- BOTÓN [-] DELETE ---
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

        // Ajustar índice tras borrar
        if (m_SelectedListIndex >= m_Lists.size()) {
            m_SelectedListIndex = std::max(0, static_cast<int>(m_Lists.size()) - 1);
        }
        if (m_Lists.empty()) m_SelectedListIndex = -1;

        m_SelectedTaskIndex = -1;
    }


    // =================================================================================
    // ZONA 3: ACCIONES DE TAREA (DERECHA)
    // =================================================================================
    const char* tabs[3] = { "New", "Save", "Delete" };

    for (int i = 0; i < 3; i++)
    {
        Rectangle btn = {
            container.x + container.width - margin - (3 - i) * (actionBtnWidth + spacing),
            container.y + (container.height - actionBtnHeight) * 0.5f,
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
            // Seguridad: Si no hay lista seleccionada, no hacemos nada
            if (m_SelectedListIndex == -1) continue;

            int currentListID = m_Lists[m_SelectedListIndex].Id;

            // ---------------------------------------------------------
            // 0. NEW TASK (Crear Nueva)
            // ---------------------------------------------------------
            if (i == 0)
            {
                TaskSpecification newTask;
                newTask.ListId = currentListID;
                newTask.Description = "";
                newTask.Status = false;

                // Guardar en BD
                auto newId = m_TaskManager->createTask(newTask);

                if (newId.has_value()) {
                    // Seleccionar la tarea recién creada
                    m_SelectedTaskIndex = newId.value();

                    // Cargar datos en los buffers de edición (Helper necesario)
                    LoadTaskToBuffer(m_SelectedTaskIndex);
                }
            }
            // ---------------------------------------------------------
            // 1. SAVE TASK (Guardar Cambios)
            // ---------------------------------------------------------
            else if (i == 1) // --- BOTÓN SAVE ---
            {
                // Solo guardamos si hay tarea y lista seleccionada
                if (m_SelectedTaskIndex != -1 && m_SelectedListIndex != -1)
                {
                    int currentListID = m_Lists[m_SelectedListIndex].Id;

                    // 1. Recuperar la tarea original de la BD para no perder datos
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

                        // DATOS NUEVOS
                        spec.Title = m_EditTitleBuffer;
                        spec.Description = m_EditContentBuffer;

                        if (m_DayBuffer.empty() || m_MonthBuffer.empty() || m_YearBuffer.empty())
                        {
                            spec.DueDate = std::chrono::system_clock::from_time_t(0);
                        }
                        else
                        {
                            std::string d = (m_DayBuffer.length() == 1) ? "0" + m_DayBuffer : m_DayBuffer;
                            std::string m = (m_MonthBuffer.length() == 1) ? "0" + m_MonthBuffer : m_MonthBuffer;
                            std::string y = m_YearBuffer; // Asumimos 4 dígitos

                            std::string dateStr = y + "-" + m + "-" + d + " 12:00:00";
                            spec.DueDate = TaskUtilities::stringToTimePoint(dateStr);
                        }

                        m_TaskManager->updateTask(spec);
                    }
                }
            }

            else if (i == 2)
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
// SEGURIDAD
    if (m_SelectedListIndex == -1 || m_SelectedListIndex >= m_Lists.size()) return;
    int currentListID = m_Lists[m_SelectedListIndex].Id;

    float padding = 20.0f;
    float panelWidth = bounds.width * 0.40f;

    // ----------------------- BARRA DE BÚSQUEDA ---------------------
    float barHeight = 70.0f;
    float barSpacing = 15.0f;
    static char searchText[128] = "";
    static bool searchEdit = false;

    Rectangle barRect = {
        bounds.x + padding + 40,
        bounds.y + padding * 6.0f - barHeight - barSpacing,
        panelWidth - padding,
        barHeight
    };

    // Estilos Textbox (Simplificado)
    GuiSetStyle(TEXTBOX, BASE_COLOR_NORMAL, 0x00000000);
    GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, 0xffffffff);
    GuiSetStyle(TEXTBOX, BORDER_COLOR_NORMAL, 0x00000000);

    DrawRectangleRounded(barRect, 0.3f, 10, Theme::BG_Sidebar);
    if (GuiTextBox(barRect, searchText, sizeof(searchText), searchEdit)) searchEdit = !searchEdit;

    // ------------------ PANEL TASKS ------------------
    Rectangle panelBounds = {
        bounds.x + padding + 40,
        bounds.y + padding * 7.0f,
        panelWidth - padding,
        bounds.height - padding * 9.0f
    };

    // 1. CARGA Y FILTRADO
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

    // --- CÁLCULOS SCROLL ---
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

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, handle)) {
            dragging = true; dragOffset = mouse.y - handle.y;
        }
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
    } else {
        scrollY = 0; dragging = false;
    }

    // FONDO Y RECORTE
    DrawRectangle(panelBounds.x, panelBounds.y, panelBounds.width, panelBounds.height, Fade(BLANK, 0));
    BeginScissorMode(panelBounds.x, panelBounds.y, panelBounds.width, panelBounds.height);

    float y = panelBounds.y - scrollY;
    Vector2 mouse = GetMousePosition();

    // ================= LOOP DE BOTONES =================
    for (const auto& item : displayItems)
    {
        Rectangle btn = { panelBounds.x + 10, y, panelBounds.width - 20, btnHeight };

        bool isSelected = (item.Id == m_SelectedTaskIndex);
        Color btnColor = isSelected ? Theme::Selection_Blue : Theme::BG_Panel;
        Color txtColor = isSelected ? WHITE : Theme::Text_Dark;

        DrawRectangleRounded(btn, 0.2f, 12, btnColor);

        // --- CHECKBOX ---
        float checkSize = 30.0f;
        float checkMargin = 20.0f;
        Rectangle checkRect = { btn.x + btn.width - checkSize - checkMargin, btn.y + (btnHeight - checkSize) / 2, checkSize, checkSize };

        DrawRectangleRounded(checkRect, 0.3f, 6, item.Status ? Theme::Border_Panel : Fade(GRAY, 0.5f));
        // FIX: Eliminado el 5to parámetro
        DrawRectangleRoundedLines(checkRect, 0.3f, 6, item.Status ? WHITE : GRAY);
        if (item.Status) DrawLabel("v", checkRect.x + 8, checkRect.y + 2, 25, WHITE);

        // --- TÍTULO (TRUNCADO PARA EVITAR SCISSOR) ---
        float textStartX = btn.x + 30;
        float maxTextWidth = (checkRect.x - 10) - textStartX;
        std::string displayTitle = item.Title;

        // Truncar texto si es muy largo para que no toque el checkbox
        if (MeasureText(displayTitle.c_str(), 30) > maxTextWidth) {
            while (displayTitle.length() > 3 && MeasureText((displayTitle + "...").c_str(), 30) > maxTextWidth) {
                displayTitle.pop_back();
            }
            displayTitle += "...";
        }

        DrawLabel(displayTitle.c_str(), textStartX, btn.y + (btnHeight - 30) / 2, 30, txtColor);

        // --- CLICKS ---
        if (CheckCollisionPointRec(mouse, checkRect)) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                TaskSpecification u = item; u.Status = !item.Status;
                m_TaskManager->updateTask(u);
            }
        } else if (CheckCollisionPointRec(mouse, btn)) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                m_SelectedTaskIndex = item.Id;
                LoadTaskToBuffer(m_SelectedTaskIndex);
            }
        }
        y += btnHeight + btnSpacing;
    }
    EndScissorMode();


    // -------------------- TASKVIEW (DERECHA) --------------------
    float viewWidth = bounds.width * 0.5f;
    Rectangle viewer = { bounds.x + bounds.width * 0.5f + 20, bounds.y + padding, bounds.width * 0.45f - 20, bounds.height - padding * 2 };

    float innerScale = 0.95f;
    Rectangle inner = { viewer.x + viewer.width * (1.0f - innerScale) / 2, viewer.y + 27 + viewer.height * (1.0f - innerScale) / 2, viewer.width * innerScale, viewer.height * innerScale - 27 };

    DrawRectangleRounded(viewer, 0.05f, 20, Theme::Border_Panel);
    DrawRectangleRounded(inner, 0.05f, 20, Theme::BG_Panel);

    // Sincronizar selección
    static int lastSelectedIndex = -1;
    if (m_SelectedTaskIndex != lastSelectedIndex) {
        if (m_SelectedTaskIndex != -1) LoadTaskToBuffer(m_SelectedTaskIndex);
        else {
            m_EditTitleBuffer.clear(); m_EditContentBuffer.clear();
            m_DayBuffer.clear(); m_MonthBuffer.clear(); m_YearBuffer.clear();
        }
        lastSelectedIndex = m_SelectedTaskIndex;
    }

    if (m_SelectedTaskIndex != -1)
    {
        // Estados de foco
        static bool titleActive = false, contentActive = false;
        static bool dayActive = false, monthActive = false, yearActive = false;

        float paddingInner = 10.0f;

        // --- LAYOUT ---
        Rectangle titleBox = { inner.x + paddingInner, inner.y + paddingInner, inner.width - paddingInner*2, 60 };
        // Reducir altura del contenido para dar espacio a la fecha
        Rectangle contentBox = { inner.x + paddingInner, inner.y + 75, inner.width - paddingInner*2, inner.height - 140 };

        // Configuración Fecha (3 Campos)
        float bottomY = inner.y + inner.height - 45;
        float boxHeight = 35.0f;
        float dayWidth = 40.0f; float monthWidth = 40.0f; float yearWidth = 60.0f; float spacing = 5.0f;

        Rectangle boxYear  = { inner.x + inner.width - yearWidth - 20, bottomY, yearWidth, boxHeight };
        Rectangle boxMonth = { boxYear.x - spacing - monthWidth, bottomY, monthWidth, boxHeight };
        Rectangle boxDay   = { boxMonth.x - spacing - dayWidth, bottomY, dayWidth, boxHeight };

        std::string contentWrapped = (m_EditContentBuffer.empty()) ? "" : WrapText(m_EditContentBuffer, 20, contentBox.width - 10);

        // --- DIBUJAR FONDOS ---
        DrawRectangleRounded(titleBox, 0.5f, 12, titleActive ? Color{200,200,200,50} : Theme::BG_Panel);
        DrawRectangleRounded(contentBox, 0.1f, 12, contentActive ? Color{200,200,200,50} : Theme::BG_Panel);

        DrawRectangleRounded(boxDay, 0.3f, 4, dayActive ? Color{230,230,250,255} : Fade(LIGHTGRAY, 0.3f));
        DrawRectangleRounded(boxMonth, 0.3f, 4, monthActive ? Color{230,230,250,255} : Fade(LIGHTGRAY, 0.3f));
        DrawRectangleRounded(boxYear, 0.3f, 4, yearActive ? Color{230,230,250,255} : Fade(LIGHTGRAY, 0.3f));

        // --- INPUT HANDLING ---
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mp = GetMousePosition();
            titleActive = CheckCollisionPointRec(mp, titleBox);
            contentActive = CheckCollisionPointRec(mp, contentBox);
            dayActive = CheckCollisionPointRec(mp, boxDay);
            monthActive = CheckCollisionPointRec(mp, boxMonth);
            yearActive = CheckCollisionPointRec(mp, boxYear);
        }

        // Tabulación entre fecha
        if (IsKeyPressed(KEY_TAB)) {
            if (dayActive) { dayActive=false; monthActive=true; }
            else if (monthActive) { monthActive=false; yearActive=true; }
            else if (yearActive) { yearActive=false; dayActive=true; }
        }

        int c;
        while ((c = GetCharPressed()) > 0) {
            if (titleActive && m_EditTitleBuffer.size() < 60) m_EditTitleBuffer += (char)c;
            else if (contentActive) m_EditContentBuffer += (char)c;
            else if (isdigit(c)) { // Solo números para fecha
                if (dayActive && m_DayBuffer.size() < 2) m_DayBuffer += (char)c;
                else if (monthActive && m_MonthBuffer.size() < 2) m_MonthBuffer += (char)c;
                else if (yearActive && m_YearBuffer.size() < 4) m_YearBuffer += (char)c;
            }
        }

        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
             if (titleActive && !m_EditTitleBuffer.empty()) m_EditTitleBuffer.pop_back();
             if (contentActive && !m_EditContentBuffer.empty()) m_EditContentBuffer.pop_back();
             if (dayActive && !m_DayBuffer.empty()) m_DayBuffer.pop_back();
             if (monthActive && !m_MonthBuffer.empty()) m_MonthBuffer.pop_back();
             if (yearActive && !m_YearBuffer.empty()) m_YearBuffer.pop_back();
        }
        if (IsKeyPressed(KEY_ENTER) && contentActive) m_EditContentBuffer += '\n';

        // --- DIBUJAR TEXTOS ---
        BeginScissorMode((int)titleBox.x, (int)titleBox.y, (int)titleBox.width, (int)titleBox.height);
            DrawLabel(m_EditTitleBuffer.c_str(), titleBox.x + 5, titleBox.y + 10, 40, BLACK);
        EndScissorMode();

        BeginScissorMode((int)contentBox.x, (int)contentBox.y, (int)contentBox.width, (int)contentBox.height);
            DrawLabel(contentWrapped.c_str(), contentBox.x + 5, contentBox.y + 5, 20, BLACK);
        EndScissorMode();

        // -----------------------------------------------------------
        //      SECCIÓN DE FECHA (CON BOTÓN CLEAR A LA IZQUIERDA)
        // -----------------------------------------------------------

        // 1. Botón Clear (X) - Calculado a la izquierda
        Rectangle btnClear = { boxDay.x - 80, bottomY + 5, 25, 25 };

        // Detectar si el mouse está encima
        bool hoverClear = CheckCollisionPointRec(GetMousePosition(), btnClear);

        // A) DIBUJAR FONDO (Rojo suave si hover, gris transparente si no)
        DrawRectangleRounded(btnClear, 0.3f, 4, hoverClear ? Fade(RED, 0.2f) : Fade(GRAY, 0.2f));

        // B) DIBUJAR BORDE (Igual que tus checkboxes)
        DrawRectangleRoundedLines(btnClear, 0.3f, 4, hoverClear ? RED : GRAY);

        // C) DIBUJAR LA "x" CENTRADA
        // Ajusta el +8 y +4 si la x no queda perfectamente centrada con tu fuente
        DrawText("x", btnClear.x + 8, btnClear.y + 4, 20, hoverClear ? RED : GRAY);

        // D) LÓGICA DE CLICK
        if (hoverClear && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            m_DayBuffer = "";
            m_MonthBuffer = "";
            m_YearBuffer = "";
        }

        // 2. Etiqueta "Due:" - Entre el botón y los inputs
        DrawLabel("Due:", boxDay.x - 45, bottomY + 8, 20, GRAY);

        // 3. Separadores "/"
        DrawLabel("/", boxDay.x + dayWidth - 3, bottomY + 8, 20, GRAY);
        DrawLabel("/", boxMonth.x + monthWidth - 3, bottomY + 8, 20, GRAY);

        // 4. Inputs de Fecha (Helper Lambda para centrar texto)
        auto DrawCentered = [this](std::string& txt, Rectangle r, const char* ph)
        {
            if (txt.empty())
                this->DrawLabel(ph, r.x + 5, r.y + 8, 20, Fade(GRAY, 0.5f));
            else
            {
                int w = MeasureText(txt.c_str(), 20);
                this->DrawLabel(txt.c_str(), r.x + (r.width - w)/2, r.y + 8, 20, BLACK);
            }
        };

        DrawCentered(m_DayBuffer, boxDay, "DD");
        DrawCentered(m_MonthBuffer, boxMonth, "MM");
        DrawCentered(m_YearBuffer, boxYear, "YYYY");

    } else {
        const char* msg = "Pick a Task";
        int w = MeasureText(msg, 20);
        DrawText(msg, inner.x + (inner.width - w)/2, inner.y + inner.height/2, 20, LIGHTGRAY);
    }
}

void AppLayer::RenderCalendarScreen(Rectangle bounds)
{
    DrawLabel("Calendar View - Coming Soon", (int)bounds.x + 50, (int)bounds.y + 50, 40, Theme::Text_Dark);
}

void AppLayer::RenderSettingsScreen(Rectangle bounds)
{
    // --- 1. FONDO ---
    const float margin = 40.0f;
    Rectangle panelRect = {
        bounds.x + margin,
        bounds.y + margin,
        bounds.width - (margin * 2),
        bounds.height - (margin * 2)
    };

    DrawRectangleRounded(panelRect, 0.02f, 10, Theme::BG_Panel);

    // --- 2. CONFIGURACIÓN DE ESTILO ---
    int defaultSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    // Helper Lambda para botones con estilo
    auto DrawStyledButton = [](Rectangle r, const char* text, Color baseColor) -> bool
    {
        int prevBase = GuiGetStyle(BUTTON, BASE_COLOR_NORMAL);
        int prevText = GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL);
        int prevBorder = GuiGetStyle(BUTTON, BORDER_COLOR_NORMAL);
        int prevAlign = GuiGetStyle(BUTTON, TEXT_ALIGNMENT);

        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(baseColor));
        GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
        GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(baseColor));
        GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER); // Asegurar texto centrado

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

    // 2. DIBUJAR
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

    DrawLabel("Appearance", (int)startX, (int)cursorY, 24, Fade(Theme::Text_Dark, 0.5f));
    cursorY += 40.0f;

    static bool isDarkMode = true;

    GuiSetStyle(TOGGLE, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    GuiSetStyle(TOGGLE, TEXT_PADDING, 0);

    DrawLabel("Dark Mode", (int)startX, (int)cursorY + 5, 20, Theme::Text_Dark);

    Rectangle toggleRect = { startX + contentWidth - 60, cursorY, 60, 30 };
    if (GuiToggle(toggleRect, isDarkMode ? "ON" : "OFF", &isDarkMode))
    {
        // Lógica de tema
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
        // 1. Iterar sobre todas las listas (Limpieza Global)
        for (const auto& list : m_Lists)
        {
            // Cargar tareas de esta lista
            auto tasks = m_TaskManager->loadTasks(list.Id);

            for (const auto& task : tasks)
            {
                // Si la tarea está marcada como completada (Status == true)
                if (task.getStatus())
                {
                    m_TaskManager->deleteTask(task.getId());
                }
            }
        }

        // 2. Resetear la selección visual por seguridad
        // (Por si la tarea que tenías seleccionada en la otra pantalla se acaba de borrar)
        m_SelectedTaskIndex = -1;

        // 3. Limpiar los buffers de edición
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