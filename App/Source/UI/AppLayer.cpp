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

std::string WrapText(const std::string& text, int fontSize, float maxWidth)
{
    std::string result;
    std::string word;
    std::string line;

    for (char c : text)
    {
        if (c == ' ' || c == '\n')
        {
            // Probar si cabe la palabra en la línea actual
            std::string testLine = line + word + c;
            if (MeasureText(testLine.c_str(), fontSize) > maxWidth)
            {
                result += line + "\n";
                line = word + c;
            }
            else
            {
                line = testLine;
            }
            word.clear();
        }
        else
        {
            word += c;
        }
    }

    // Última palabra
    if (!word.empty())
    {
        if (MeasureText((line + word).c_str(), fontSize) > maxWidth)
        {
            result += line + "\n" + word;
        }
        else
        {
            result += line + word;
        }
    }

    return result;
}

void AppLayer::RenderTasksScreen(Rectangle bounds)
{
    int taskCount = 20;

    // ------------------------------ LISTA DE TASKS ------------------------------

    float padding = 20.0f;
    float panelWidth = bounds.width * 0.40f;

    // ----------------------- BARRA DE TEXTO ENCIMA DEL PANEL ---------------------
    // ----------------------- BARRA DE TEXTO ENCIMA DEL PANEL ---------------------

    float barHeight = 70.0f;
    float barSpacing = 15.0f;
    static char searchText[128] = "";
    static bool searchEdit = false;

    Rectangle barRect = {
        bounds.x + padding + 40,
        bounds.y + padding * 7.0f - barHeight - barSpacing,
        panelWidth - padding,
        barHeight
    };

    // Hacer la textbox y  transparente y el texto blanco
    GuiSetStyle(TEXTBOX, BASE_COLOR_NORMAL, 0x00000000);
    GuiSetStyle(TEXTBOX, BASE_COLOR_PRESSED, 0x00000000);
    GuiSetStyle(TEXTBOX, BASE_COLOR_FOCUSED, 0x00000000);

    GuiSetStyle(TEXTBOX,BORDER_COLOR_NORMAL, 0x00000000);
    GuiSetStyle(TEXTBOX,BORDER_COLOR_PRESSED, 0x00000000);
    GuiSetStyle(TEXTBOX,BORDER_COLOR_FOCUSED, 0x00000000);

    GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, 0xffffffff);
    GuiSetStyle(TEXTBOX, TEXT_COLOR_FOCUSED, 0xffffffff);
    GuiSetStyle(TEXTBOX, TEXT_COLOR_PRESSED, 0xffffffff);

    // --- FONDO REAL DETRÁS DEL TEXTBOX ---
    DrawRectangleRounded(barRect,0.3f, 10, Theme::BG_Sidebar);

    // --- DIBUJAR TEXTBOX ENCIMA DEL FONDO ---
    if (GuiTextBox(barRect, searchText, sizeof(searchText), searchEdit)) {
        searchEdit = !searchEdit;
    }

    // ------------- PANEL TASKS --------------
    // Todo es proporcional de aqui asi que tomenlo en cuenta
    Rectangle panelBounds = {
        bounds.x + padding + 40
        ,bounds.y + padding * 7.0f,
        panelWidth - padding,
        bounds.height - padding * 9.0f
    };

    float btnHeight = 70.0f;
    float btnSpacing = 15.0f;

    float contentHeight = taskCount * (btnHeight + btnSpacing) + 50;

    // ---------- Scroll -----------
    static float scrollY = 0;
    static bool dragging = false;
    static float dragOffset = 0;

    // Scrollbar
    float scrollbarWidth = 15.0f;
    Rectangle scrollBar = {
        panelBounds.x - scrollbarWidth - 5,
        panelBounds.y,
        scrollbarWidth,
        panelBounds.height
    };

    float handleHeight = panelBounds.height * panelBounds.height / contentHeight;
    if (handleHeight < 20) handleHeight = 20;
    if (handleHeight > panelBounds.height) handleHeight = panelBounds.height;

    float handleY = panelBounds.y + scrollY * (panelBounds.height - handleHeight) / (contentHeight - panelBounds.height);
    Rectangle handle = { scrollBar.x, handleY, scrollBar.width, handleHeight };

    // Dibujar scrollbar
    DrawRectangleRounded(handle,20.0f, 10, BLACK);

    // Interacción
    Vector2 mouse = GetMousePosition();

    // Iniciar arrastre si se clickea el handle
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, handle))
    {
        dragging = true;
        dragOffset = mouse.y - handle.y; // offset para que no salte
    }

    // Terminar arrastre
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    {
        dragging = false;
    }

    // Arrastrar aunque el mouse esté fuera del scrollbar
    if (dragging)
    {
        scrollY = (mouse.y - panelBounds.y - dragOffset) * (contentHeight - panelBounds.height) / (panelBounds.height - handleHeight);
        if (scrollY < 0) scrollY = 0;
        if (scrollY > contentHeight - panelBounds.height) scrollY = contentHeight - panelBounds.height;
    }

    // --- Scroll con rueda del mouse ---
    float wheel = GetMouseWheelMove();
    if (wheel != 0)
    {
        scrollY -= wheel * 30; // velocidad del scroll
        if (scrollY < 0) scrollY = 0;
        if (scrollY > contentHeight - panelBounds.height) scrollY = contentHeight - panelBounds.height;
    }
    DrawRectangle(panelBounds.x, panelBounds.y, panelBounds.width, panelBounds.height, Fade(BLANK, 0));

    // Área recortada para contenido
    BeginScissorMode(panelBounds.x, panelBounds.y, panelBounds.width, panelBounds.height);

    // ---------- Loop de botones ----------
    float y = panelBounds.y - scrollY;
    for (int i = 0; i < taskCount; i++)
    {
        Rectangle btn = { panelBounds.x + 10, y, panelBounds.width - 20, btnHeight };
        DrawRectangleRounded(btn, 0.2f, 12, Theme::BG_Panel); // 0.2f = radio, 12 segmentos

        //Aqui se asigna el nombre
        std::string taskName = "Tarea " + std::to_string(i + 1);

        // Texto centrado
        int fontSize = 30;
        int textWidth = MeasureText(TextFormat(taskName.c_str()), fontSize);
        int textX = btn.x + (btn.width - textWidth) / 2;
        int textY = btn.y + (btn.height - fontSize) / 2;
        DrawLabel(TextFormat(taskName.c_str()), textX, textY, fontSize, Theme::Text_Dark);

        y += btnHeight + btnSpacing;
    }
    EndScissorMode();


    // -------------- Boton Search ----------------

    // --------------------------------------------

    // ------------------------------ TASKVIEW   ------------------------------

    // --- Base ---
    float viewWidth = bounds.width * 0.5f;

    Rectangle viewer = {
        bounds.x + bounds.width * 0.5f + 20, // empieza después del panel lateral
        bounds.y + padding,
        bounds.width * 0.45f - 20,           // 45% del ancho restante proporcional a la ventana
        bounds.height - padding * 2
    };

    float innerScale = 0.95f; // <-- Cambia este valor entre 0.0 y 1.0 para ajustar tamaño
    Rectangle inner = {
        viewer.x + viewer.width * (1.0f - innerScale) / 2,   // centrado horizontal
        viewer.y + 27 + viewer.height * (1.0f - innerScale) / 2,  // centrado vertical
        viewer.width * innerScale,
        viewer.height * innerScale - 27
    };

    DrawRectangleRounded(viewer,0.1f, 10, Theme::Border_Panel);
    DrawRectangleRounded(inner, 0.1f, 12, Theme::BG_Panel);

    // --- Contenido ---
    // Variables
    static std::string titleText = "Título de la tarea";
    static std::string contentText = "Contenido de la tarea largo...";
    static bool titleActive = false;
    static bool contentActive = false;

    float paddingInner = 10.0f;

    // Cajas
    Rectangle titleBox = { inner.x + paddingInner, inner.y + paddingInner, inner.width - paddingInner*2, 40 };
    Rectangle contentBox = { inner.x + paddingInner, inner.y + 60, inner.width - paddingInner*2, inner.height - 70 };

    std::string contentTextWrapped = WrapText(contentText, 20, contentBox.width - 10);

    // Dibujar fondo de las cajas
    DrawRectangleRounded(titleBox, 0.5f, 12, titleActive ? Color (200,200,200,50):Theme::BG_Panel);
    DrawRectangleRounded(contentBox, 0.1f, 12, contentActive ? Color (200,200,200,50):Theme::BG_Panel);

    // Detectar click para activar
    mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        titleActive = CheckCollisionPointRec(mouse, titleBox);
        contentActive = CheckCollisionPointRec(mouse, contentBox);
    }

    // Capturar caracteres presionados
    int c;
    while ((c = GetCharPressed()) > 0) {
        if (titleActive) {
            if (c >= 32 && c <= 125) titleText += (char)c;
        } else if (contentActive) {
            if (c >= 32 && c <= 125) contentText += (char)c;
        }
    }

    // Detectar ENTER para salto de línea
    if (IsKeyPressed(KEY_ENTER))
    {
        if (titleActive)
            titleText += '\n';
        else if (contentActive)
            contentText += '\n';
    }

    // Borrar con Backspace
    static float backspaceTimer = 0.0f;
    static const float backspaceDelay = 0.1f; // segundos entre cada borrado

    // Dentro de tu loop de renderizado (por frame):
    float dt = GetFrameTime(); // tiempo del frame actual

    if (IsKeyDown(KEY_BACKSPACE)) {
        backspaceTimer -= dt;
        if (backspaceTimer <= 0.0f) {
            // Borra el último carácter según la caja activa
            if (titleActive && !titleText.empty()) titleText.pop_back();
            if (contentActive && !contentText.empty()) contentText.pop_back();

            backspaceTimer = backspaceDelay; // reinicia el timer
        }
    } else {
        backspaceTimer = 0.0f; // resetear cuando se suelta
    }


    // Dibujar texto dentro de las cajas (arriba-izquierda)
    DrawLabel(titleText.c_str(), titleBox.x + 5, titleBox.y + 5, 50, BLACK);
    DrawLabel(contentTextWrapped.c_str(), contentBox.x + 5, contentBox.y + 5, 20, BLACK);
}



void AppLayer::RenderBrowseScreen(Rectangle bounds)
{
    Rectangle rect = { 100, 100, 200, 80 };
    DrawRectangleRounded(rect, 0.2f, 16, WHITE);


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