#ifndef AUTHLAYER_H
#define AUTHLAYER_H
#include "Core/Layer.h"


class AuthLayer : Core::Layer {
public:
    AuthLayer() = default;
    ~AuthLayer() override = default;

    void OnEvent(Event& event) override;
    void OnUpdate(float ts) override;
    void OnRender() override;
};



#endif //AUTHLAYER_H
