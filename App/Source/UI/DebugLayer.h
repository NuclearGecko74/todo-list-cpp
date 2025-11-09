#ifndef TODOLIST_DEBUGLAYER_H
#define TODOLIST_DEBUGLAYER_H

#include "Core/Layer.h"

class DebugLayer : public Core::Layer {
public:
    DebugLayer();

    ~DebugLayer() override;

    virtual void OnEvent(Event& event) override;
    virtual void OnUpdate(float ts) override;
    virtual void OnRender() override;
};


#endif