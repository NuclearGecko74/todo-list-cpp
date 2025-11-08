#ifndef TODOLIST_APPLAYER_H
#define TODOLIST_APPLAYER_H

#include "Core/Layer.h"
#include <string>
#include <vector>
#include <utility>

class AppLayer : public Core::Layer {
public:
    AppLayer();
    virtual ~AppLayer();

    virtual void OnEvent(Event& event) override;
    virtual void OnUpdate(float ts) override;
    virtual void OnRender() override;
};


#endif