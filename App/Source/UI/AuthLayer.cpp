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
    if (IsKeyPressed(KEY_ENTER))
    {
        AttemptLogin();
    }
}

void AuthLayer::OnRender()
{
    const float kCardWidth = 800.0f;
    const float kCardHeight = 500.0f;

    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();

    // Center calculation
    float cardX = (screenW - kCardWidth) / 2.0f;
    float cardY = (screenH - kCardHeight) / 2.0f;
    Rectangle cardRect = { cardX, cardY, kCardWidth, kCardHeight };

    ClearBackground(Theme::BG_Main);

    DrawCardBackground(cardRect);
    DrawLoginForm(cardRect);
}

void AuthLayer::DrawCardBackground(Rectangle cardRect)
{
    const float borderSize = 20.0f;

    // Draw Border (simulated by a larger rectangle behind)
    Rectangle borderRect = {
        cardRect.x - borderSize,
        cardRect.y - borderSize,
        cardRect.width + (borderSize * 2),
        cardRect.height + (borderSize * 2)
    };

    DrawRectangleRounded(borderRect, 0.05f, 10, Theme::BG_Sidebar);
    DrawRectangleRounded(cardRect, 0.05f, 10, Theme::BG_Panel);
}

void AuthLayer::DrawLoginForm(Rectangle cardRect)
{
    const float contentPadding = 50.0f;
    const float inputWidth = 300.0f;
    const float inputHeight = 40.0f;
    const float inputSpacing = 60.0f;

    float startX = cardRect.x + contentPadding;
    float currentY = cardRect.y + 60.0f;

    // Title
    DrawTextEx(AppResources::GetFont(), "Sign in", { startX, currentY }, 60, 0, Theme::BG_Sidebar);

    currentY += 100.0f;

    // Username Input
    DrawInputField(
        { startX, currentY, inputWidth, inputHeight },
        m_UsernameBuffer,
        128,
        m_EditUsername,
        "Username..."
    );

    currentY += inputSpacing;

    // Password Input
    DrawInputField(
        { startX, currentY, inputWidth, inputHeight },
        m_PasswordBuffer,
        128,
        m_EditPassword,
        "Password..."
    );

    // Signup Link
    GuiLabel({ startX, currentY + 45.0f, inputWidth, 20.0f }, "Don't have an account? Sign up!");
}

void AuthLayer::DrawInputField(Rectangle bounds, char* buffer, int bufferSize, bool& editMode, const char* placeholder)
{
    if (GuiTextBox(bounds, buffer, bufferSize, editMode))
    {
        editMode = !editMode;
    }

    // Draw placeholder if empty and not focused
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
    // TODO: Connect to database
    bool credentialsValid = true;

    if (credentialsValid)
    {
        TransitionTo<AppLayer>();
    }
}