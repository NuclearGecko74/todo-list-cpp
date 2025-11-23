#include "Layer.h"

#include "Application.h"

namespace Core {

    void Layer::QueueTransition(std::unique_ptr<Layer> toLayer)
    {
        auto pendingLayerWrapper = std::make_shared<std::unique_ptr<Layer>>(std::move(toLayer));

        Application::Get().SubmitPostFrameAction([this, pendingLayerWrapper]() {

            auto& layerStack = Application::Get().m_LayerStack;

            for (auto& layer : layerStack)
            {
                if (layer.get() == this)
                {
                    layer = std::move(*pendingLayerWrapper);
                    return;
                }
            }
        });
    }
}