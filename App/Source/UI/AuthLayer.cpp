#include "AuthLayer.h"

#include "AppLayer.h"
#include "raygui.h"
#include "Theme.h"
#include "Core/Application.h"

AuthLayer::AuthLayer()
{
    m_AuthManager = std::make_unique<AuthManager>(AppResources::GetDatabase());
}

void AuthLayer::OnUpdate(float ts)
{
    if (m_CurrentMode != AuthMode::Welcome && IsKeyPressed(KEY_ENTER))
    {
        HandleEnterKey();
    }
}

void AuthLayer::HandleEnterKey()
{
    if (m_CurrentMode == AuthMode::Login) AttemptLogin();
    else if (m_CurrentMode == AuthMode::Register) AttemptRegister();
}

static bool DrawStyledButton(Rectangle bounds, const char* text)
{
    // 1. Guardar estilos originales
    int prevBase = GuiGetStyle(BUTTON, BASE_COLOR_NORMAL);
    int prevText = GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL);
    int prevBorder = GuiGetStyle(BUTTON, BORDER_COLOR_NORMAL);

    // 2. Aplicar estilo "RizzList" (Fondo Oscuro, Texto Blanco)
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(Theme::BG_Sidebar));
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(Theme::BG_Sidebar)); // Sin borde visible

    // 3. Dibujar
    bool clicked = GuiButton(bounds, text);

    // 4. Restaurar estilos
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, prevBase);
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, prevText);
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, prevBorder);

    return clicked;
}

void AuthLayer::OnRender()
{
    const float kCardWidth = 800.0f;

    // Altura dinámica: Welcome es más pequeña, Register es más alta
    float kCardHeight = 500.0f;
    if (m_CurrentMode == AuthMode::Welcome) kCardHeight = 450.0f;
    if (m_CurrentMode == AuthMode::Register) kCardHeight = 580.0f;

    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();

    // Centrado
    float cardX = (screenW - kCardWidth) / 2.0f;
    float cardY = (screenH - kCardHeight) / 2.0f;
    Rectangle cardRect = { cardX, cardY, kCardWidth, kCardHeight };

    ClearBackground(Theme::BG_Main);

    DrawCardBackground(cardRect);

    switch (m_CurrentMode)
    {
    case AuthMode::Welcome:
        DrawWelcomeContent(cardRect);
        break;
    case AuthMode::Login:
    case AuthMode::Register:
        DrawFormContent(cardRect);
        break;
    }
}

void AuthLayer::DrawCardBackground(Rectangle cardRect)
{
    const float borderSize = 20.0f;

    // Borde simulado (rectángulo más grande atrás)
    Rectangle borderRect = {
        cardRect.x - borderSize,
        cardRect.y - borderSize,
        cardRect.width + (borderSize * 2),
        cardRect.height + (borderSize * 2)
    };

    DrawRectangleRounded(borderRect, 0.05f, 10, Theme::BG_Sidebar);
    DrawRectangleRounded(cardRect, 0.05f, 10, Theme::BG_Panel);
}

void AuthLayer::DrawWelcomeContent(Rectangle cardRect)
{
    float centerX = cardRect.x + (cardRect.width / 2.0f);

    // Ajuste vertical para que quede centrado visualmente
    float startY = cardRect.y + 80.0f;

    // 1. Título Grande
    const char* title = "RizzList";
    // Usamos tamaño 80 para impacto
    Vector2 textSize = MeasureTextEx(AppResources::GetFont(), title, 80, 0);
    DrawTextEx(AppResources::GetFont(), title,
               { centerX - (textSize.x / 2.0f), startY },
               80, 0, Theme::BG_Sidebar);

    // 2. Subtítulo
    startY += 90.0f;
    const char* subtitle = "Organize your rizz.";
    Vector2 subSize = MeasureTextEx(AppResources::GetFont(), subtitle, 24, 0);
    DrawTextEx(AppResources::GetFont(), subtitle,
               { centerX - (subSize.x / 2.0f), startY },
               24, 0, LIGHTGRAY);

    // 3. Botones Grandes
    startY += 80.0f;
    float btnWidth = 300.0f;
    float btnHeight = 50.0f; // Más altos para verse modernos
    float btnX = centerX - (btnWidth / 2.0f);

    // Botón Login (Estilizado)
    if (DrawStyledButton({ btnX, startY, btnWidth, btnHeight }, "Log In"))
    {
        m_CurrentMode = AuthMode::Login;
    }

    startY += 70.0f; // Espacio entre botones

    // Botón Sign Up (Estilizado)
    if (DrawStyledButton({ btnX, startY, btnWidth, btnHeight }, "Sign Up"))
    {
        m_CurrentMode = AuthMode::Register;
    }
}

void AuthLayer::DrawFormContent(Rectangle cardRect)
{
    float btnSize = 30.0f;
    float padding = 20.0f;

    Rectangle backBtnRect = {
        cardRect.x + cardRect.width - btnSize - padding,
        cardRect.y + padding,
        btnSize,
        btnSize
    };

    if (GuiButton(backBtnRect, "#159#"))
    {
        m_CurrentMode = AuthMode::Welcome;
        m_PasswordBuffer[0] = '\0';
        m_ErrorMessage.clear();
    }

    const float inputWidth = 350.0f;
    const float inputHeight = 45.0f;
    const float inputSpacing = 65.0f;

    float startX = cardRect.x + (cardRect.width - inputWidth) / 2.0f;
    float currentY = cardRect.y + 60.0f;

    const char* title = (m_CurrentMode == AuthMode::Login) ? "Welcome Back" : "Create Account";
    Vector2 titleSize = MeasureTextEx(AppResources::GetFont(), title, 50, 0);

    DrawTextEx(AppResources::GetFont(), title,
              { cardRect.x + (cardRect.width - titleSize.x)/2.0f, currentY },
              50, 0, Theme::BG_Sidebar);

    currentY += 90.0f;

    DrawInputField({ startX, currentY, inputWidth, inputHeight }, m_UsernameBuffer, 128, m_EditUsername, "Username...");
    currentY += inputSpacing;

    if (m_CurrentMode == AuthMode::Register)
    {
        DrawInputField({ startX, currentY, inputWidth, inputHeight }, m_EmailBuffer, 128, m_EditEmail, "Email...");
        currentY += inputSpacing;
    }

    DrawInputField({ startX, currentY, inputWidth, inputHeight }, m_PasswordBuffer, 128, m_EditPassword, "Password...");
    currentY += inputSpacing;

    // --- CORRECCIÓN AQUÍ: Aumentamos espacio para el mensaje de error ---
    currentY += 20.0f;

    if (!m_ErrorMessage.empty())
    {
        Vector2 textSize = MeasureTextEx(AppResources::GetFont(), m_ErrorMessage.c_str(), 20, 0);
        float textX = cardRect.x + (cardRect.width - textSize.x) / 2.0f;
        // Dibujamos el texto un poco más arriba del botón (30px arriba)
        DrawTextEx(AppResources::GetFont(), m_ErrorMessage.c_str(), { textX, currentY - 30.0f }, 20, 0, MAROON);
    }

    const char* actionText = (m_CurrentMode == AuthMode::Login) ? "Log In" : "Sign Up";

    if (DrawStyledButton({ startX, currentY, inputWidth, 50.0f }, actionText))
    {
        HandleEnterKey();
    }

    currentY += 60.0f;
    const char* switchText = (m_CurrentMode == AuthMode::Login)
        ? "Need an account? Sign up!"
        : "Have an account? Log in!";

    Vector2 textSize = MeasureTextEx(AppResources::GetFont(), switchText, 20, 0);
    float textX = cardRect.x + (cardRect.width - textSize.x) / 2.0f;

    Rectangle linkRect = { textX, currentY, textSize.x, 20.0f };

    DrawTextEx(AppResources::GetFont(), switchText, {textX, currentY}, 20, 0, GRAY);

    if (CheckCollisionPointRec(GetMousePosition(), linkRect))
    {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        DrawRectangle(linkRect.x, linkRect.y + linkRect.height, linkRect.width, 1, GRAY);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            m_CurrentMode = (m_CurrentMode == AuthMode::Login) ? AuthMode::Register : AuthMode::Login;
            m_PasswordBuffer[0] = '\0';
            m_ErrorMessage.clear();
        }
    }
    else
    {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }
}

void AuthLayer::DrawLoginForm(Rectangle cardRect)
{
    constexpr float contentPadding = 50.0f;
    constexpr float inputWidth = 300.0f;
    constexpr float inputHeight = 40.0f;
    constexpr float inputSpacing = 60.0f;

    float startX = cardRect.x + contentPadding;
    float currentY = cardRect.y + 60.0f;

    DrawTextEx(AppResources::GetFont(), "Sign in", { startX, currentY }, 60, 0, Theme::BG_Sidebar);

    currentY += 100.0f;

    DrawInputField(
        { startX, currentY, inputWidth, inputHeight },
        m_UsernameBuffer,
        128,
        m_EditUsername,
        "Username..."
    );

    currentY += inputSpacing;

    DrawInputField(
        { startX, currentY, inputWidth, inputHeight },
        m_PasswordBuffer,
        128,
        m_EditPassword,
        "Password..."
    );

    GuiLabel({ startX, currentY + 45.0f, inputWidth, 20.0f }, "Don't have an account? Sign up!");
}

void AuthLayer::DrawInputField(Rectangle bounds, char* buffer, int bufferSize, bool& editMode, const char* placeholder)
{
    if (GuiTextBox(bounds, buffer, bufferSize, editMode))
    {
        editMode = !editMode;
    }

    if (!editMode && buffer[0] == '\0')
    {
        DrawTextEx(
            AppResources::GetFont(),
            placeholder,
            { bounds.x + 8, bounds.y + 12 }, // Ajuste fino para centrar verticalmente placeholder
            20,
            0,
            Fade(LIGHTGRAY, 0.8f)
        );
    }
}

void AuthLayer::AttemptLogin()
{
    if (m_UsernameBuffer[0] == '\0' || m_PasswordBuffer[0] == '\0')
    {
        m_ErrorMessage = "Please fill in all fields.";
        return;
    }

    const int userId = m_AuthManager->login(m_UsernameBuffer, m_PasswordBuffer);

    if (userId != -1)
    {
        AppResources::SetUserId(userId);
        AppResources::SetUsername(std::string(m_UsernameBuffer));
        AppResources::SaveSession(userId);
        TransitionTo<AppLayer>();
    }
    else
    {
        m_ErrorMessage = "Invalid username or password.";
        m_PasswordBuffer[0] = '\0';
    }
}

void AuthLayer::AttemptRegister()
{
    if (m_UsernameBuffer[0] == '\0' || m_PasswordBuffer[0] == '\0')
    {
        m_ErrorMessage = "Please fill in all fields.";
        return;
    }

    if (m_AuthManager->registerUser(m_UsernameBuffer, m_PasswordBuffer))
    {
        AttemptLogin();
    }
    else
    {
        m_ErrorMessage = "Username already taken.";
    }
}