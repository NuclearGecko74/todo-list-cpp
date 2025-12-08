#ifndef AUTHLAYER_H
#define AUTHLAYER_H

#include "BaseLayer.h"
#include "Core/Layer.h"

#include "Model/AuthManager.h"

class AuthLayer : public BaseLayer {
private:
    enum class AppScreen {
        Login = 0,
        Register
    };
public:
    AuthLayer();
    virtual ~AuthLayer() = default;

    virtual void OnUpdate(float ts) override;
    virtual void OnRender() override;

private:
    void AttemptLogin();

    void DrawCardBackground(Rectangle cardRect);
    void DrawLoginForm(Rectangle cardRect);
    void DrawInputField(Rectangle bounds, char* buffer, int bufferSize, bool& editMode, const char* placeholder);

private:
    AppScreen m_CurrentScreen = AppScreen::Login;

    char m_UsernameBuffer[128] = "";
    char m_PasswordBuffer[128] = "";

    bool m_EditUsername = false;
    bool m_EditPassword = false;

    std::unique_ptr<AuthManager> m_AuthManager;
};

#endif
