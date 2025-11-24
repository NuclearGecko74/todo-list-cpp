#ifndef TODOLIST_DEBUGLAYER_H
#define TODOLIST_DEBUGLAYER_H

#include "BaseLayer.h"
#include "Core/Layer.h"

class DebugLayer : public BaseLayer {
public:
    DebugLayer();

    ~DebugLayer() override;

    virtual void OnEvent(Event& event) override;
    virtual void OnUpdate(float ts) override;
    virtual void OnRender() override;
};


#endif