#ifndef TODOLIST_DEBUGLAYER_H
#define TODOLIST_DEBUGLAYER_H

#include "BaseLayer.h"
#include "Core/Layer.h"

class DebugLayer : public BaseLayer {
public:
    DebugLayer() = default;
    ~DebugLayer() override = default;

    virtual void OnRender() override;
};


#endif