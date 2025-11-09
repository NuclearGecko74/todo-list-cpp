#ifndef TODOLIST_LAYER_H
#define TODOLIST_LAYER_H

#include "Event.h"

namespace Core {
    class Layer {
    public:
        virtual ~Layer() = default;

        virtual void OnEvent(Event& event) {}

        virtual void OnUpdate(float ts) {}

        virtual void OnRender() {}
    };
}

#endif