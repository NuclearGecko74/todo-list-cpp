#ifndef AUTHLAYER_H
#define AUTHLAYER_H

#include "BaseLayer.h"
#include "Core/Layer.h"

#include "Model/AuthManager.h"

class AuthLayer : public BaseLayer
{
public:
    AuthLayer();
    virtual ~AuthLayer() = default;

    virtual void OnUpdate(float ts) override;
    virtual void OnRender() override;

private:
    void HandleEnterKey();
    void AttemptLogin();
    void AttemptRegister();

    void DrawCardBackground(Rectangle cardRect);
    void DrawWelcomeContent(Rectangle cardRect);
    void DrawFormContent(Rectangle cardRect);
    void DrawLoginForm(Rectangle cardRect);
    void DrawInputField(Rectangle bounds, char* buffer, int bufferSize, bool& editMode, const char* placeholder);

private:
    enum class AuthMode { Welcome, Login, Register };

    AuthMode m_CurrentMode = AuthMode::Welcome;

    char m_UsernameBuffer[128] = "";
    char m_EmailBuffer[128] = "";
    char m_PasswordBuffer[128] = "";

    bool m_EditUsername = false;
    bool m_EditEmail = false;
    bool m_EditPassword = false;

    std::unique_ptr<AuthManager> m_AuthManager;

    std::string m_ErrorMessage;
};

#endif
