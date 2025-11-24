#include "AuthLayer.h"

#include "AppLayer.h"
#include "raygui.h"
#include "Theme.h"
#include "Core/Application.h"

void AuthLayer::OnUpdate(float ts)
{
    if (IsKeyPressed(KEY_ENTER))
    {
        AttemptLogin();
    }
}

void AuthLayer::OnRender()
{
    float screenW = static_cast<float>(GetScreenWidth());
    float screenH = static_cast<float>(GetScreenHeight());

    float cardWidth = 800.0f;
    float cardHeight = 500.0f;

    float cardX = (screenW - cardWidth) / 2.0f;
    float cardY = (screenH - cardHeight) / 2.0f;

    Rectangle cardRect = { cardX, cardY, cardWidth, cardHeight };

    ClearBackground(Theme::BG_Main);

    float borderSize = 20.0f;
    DrawRectangleRounded(
        { cardX - borderSize, cardY - borderSize, cardWidth + borderSize * 2, cardHeight + borderSize * 2 },
        0.05f, 10, Theme::BG_Sidebar
    );

    DrawRectangleRounded(cardRect, 0.05f, 10, Theme::BG_Panel);

    float contentPadding = 50.0f;
    float leftColumnX = cardX + contentPadding;
    float startY = cardY + 60.0f;

    Font font = AppResources::GetFont();

    DrawTextEx(font, "Sign in", { leftColumnX, startY }, 60, 0, Theme::BG_Sidebar);
    
    float inputY = startY + 100.0f;
    if (GuiTextBox({ leftColumnX, inputY, 300, 40 }, m_UsernameBuffer, 128, m_EditUsername))
    {
        m_EditUsername = !m_EditUsername;
    }

    if (!m_EditUsername && m_UsernameBuffer[0] == '\0')
    {
        DrawTextEx(font, "Username...", { leftColumnX + 5, inputY + 10 }, 20, 0, LIGHTGRAY);
    }

    inputY += 60.0f;
    if (GuiTextBox({ leftColumnX, inputY, 300, 40 }, m_PasswordBuffer, 128, m_EditPassword))
    {
        m_EditPassword = !m_EditPassword;
    }

    if (!m_EditPassword && m_PasswordBuffer[0] == '\0')
    {
        DrawTextEx(font, "Password...", { leftColumnX + 5, inputY + 10 }, 20, 0, LIGHTGRAY);
    }

    GuiLabel({ leftColumnX, inputY + 45, 300, 20 }, "Don't have an account? Sign up!");
}

void AuthLayer::AttemptLogin()
{
    // TODO Conectar base de datos
    bool credentialsValid = true;

    if (credentialsValid)
    {
        TransitionTo<AppLayer>();
    }
    else
    {
        // m_ShowError = true;
    }
}
