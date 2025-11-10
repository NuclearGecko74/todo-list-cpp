#ifndef TODOLIST_APPLAYER_H
#define TODOLIST_APPLAYER_H

#include "Core/Layer.h"

class AppLayer : public Core::Layer {
private:
    enum class AppScreen {
        Tasks,
        Settings,
        Calendar
    };
public:
    AppLayer() = default;
    ~AppLayer() override = default;

    void OnEvent(Event& event) override;
    void OnUpdate(float ts) override;
    void OnRender() override;

private:
    void RenderBackground();
    void RenderNavigationBar();

private:
    AppScreen m_CurrentScreen = AppScreen::Tasks;
};


#endif