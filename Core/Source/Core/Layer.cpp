#include "Layer.h"

#include "Application.h"

namespace Core {

    void Layer::QueueTransition(std::unique_ptr<Layer> toLayer)
    {
        Application::Get().SubmitPostFrameAction([this, newLayer = std::move(toLayer)]() mutable {

            auto& layerStack = Application::Get().GetLayerStack();

            for (auto& layer : layerStack)
            {
                if (layer.get() == this)
                {
                    layer = std::move(newLayer);
                    return;
                }
            }
        });
    }

}