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

void AuthLayer::OnRender()
{
    constexpr float kCardWidth = 800.0f;

    float kCardHeight = 500.0f;
    if (m_CurrentMode == AuthMode::Register) kCardHeight = 580.0f;
    if (m_CurrentMode == AuthMode::Welcome) kCardHeight = 450.0f;

    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();

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
    float currentY = cardRect.y + 60.0f;

    const char* title = "RizzList";
    Vector2 textSize = MeasureTextEx(AppResources::GetFont(), title, 80, 0);
    DrawTextEx(AppResources::GetFont(), title,
               { centerX - (textSize.x / 2.0f), currentY },
               80, 0, Theme::BG_Sidebar);

    currentY += 90.0f;
    const char* subtitle = "Organize your rizz.";
    Vector2 subSize = MeasureTextEx(AppResources::GetFont(), subtitle, 24, 0);
    DrawTextEx(AppResources::GetFont(), subtitle,
               { centerX - (subSize.x / 2.0f), currentY },
               24, 0, LIGHTGRAY);

    currentY += 100.0f;
    float btnWidth = 300.0f;
    float btnHeight = 50.0f;
    float btnX = centerX - (btnWidth / 2.0f);

    if (GuiButton({ btnX, currentY, btnWidth, btnHeight }, "Login"))
    {
        m_CurrentMode = AuthMode::Login;
    }

    currentY += 70.0f;

    if (GuiButton({ btnX, currentY, btnWidth, btnHeight }, "Sign Up"))
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
    }

    constexpr float contentPadding = 50.0f;
    constexpr float inputWidth = 300.0f;
    constexpr float inputHeight = 40.0f;
    constexpr float inputSpacing = 60.0f;

    float startX = cardRect.x + contentPadding;
    float currentY = cardRect.y + 50.0f;

    const char* title = (m_CurrentMode == AuthMode::Login) ? "Welcome Back" : "Create Account";
    DrawTextEx(AppResources::GetFont(), title, { startX, currentY }, 50, 0, Theme::BG_Sidebar);

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

    const char* actionText = (m_CurrentMode == AuthMode::Login) ? "Log In" : "Sign Up";
    if (GuiButton({ startX, currentY, inputWidth, 40 }, actionText))
    {
        HandleEnterKey();
    }

    currentY += 50.0f;
    const char* switchText = (m_CurrentMode == AuthMode::Login)
        ? "Need an account? Sign up!"
        : "Have an account? Log in!";

    Rectangle linkRect = { startX, currentY, inputWidth, 20.0f };
    GuiLabel(linkRect, switchText);

    if (CheckCollisionPointRec(GetMousePosition(), linkRect))
    {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            m_CurrentMode = (m_CurrentMode == AuthMode::Login) ? AuthMode::Register : AuthMode::Login;
            m_PasswordBuffer[0] = '\0';
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
            { bounds.x + 5, bounds.y + 10 },
            20,
            0,
            LIGHTGRAY
        );
    }
}

void AuthLayer::AttemptLogin()
{
    const int userId = m_AuthManager->login(m_UsernameBuffer, m_PasswordBuffer);
    if (userId == -1) return;

    AppResources::SetUserId(userId);
    TransitionTo<AppLayer>();
}

void AuthLayer::AttemptRegister()
{
    // Aqui va tu chamba chepe
}