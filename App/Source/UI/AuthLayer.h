#ifndef AUTHLAYER_H
#define AUTHLAYER_H

#include "BaseLayer.h"
#include "Core/Layer.h"


class AuthLayer : public BaseLayer {
public:
    AuthLayer() = default;
    ~AuthLayer() override = default;

    void OnUpdate(float ts) override;
    void OnRender() override;

private:
    void AttemptLogin();

private:
    char m_UsernameBuffer[128] = "";
    char m_PasswordBuffer[128] = "";

    bool m_EditUsername = false;
    bool m_EditPassword = false;
};



#endif
