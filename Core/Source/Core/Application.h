#ifndef TODOLIST_APPLICATION_H
#define TODOLIST_APPLICATION_H

#include "Layer.h"
#include "Window.h"

#include "raylib.h"

#include <string>
#include <memory>
#include <vector>
#include <set>

namespace Core {
    struct ApplicationSpecification {
        std::string Name;
        WindowSpecification WindowSpec;
    };

    class Application {
    public:
        Application(const ApplicationSpecification& specification = ApplicationSpecification());
        ~Application();

        void Run();
        void Stop();

        template<typename TLayer>
        requires(std::is_base_of_v<Layer, TLayer>)
        void PushLayer()
        {
            m_LayerStack.push_back(std::make_unique<TLayer>());
        }

        Vector2 GetFramebufferSize() const;

        static Application& Get();

        static float GetAppTime();
    private:
        ApplicationSpecification m_Specification;
        std::shared_ptr<Window> m_Window;
        bool m_Running = false;

        std::vector<std::unique_ptr<Layer>> m_LayerStack;
    };
}



#endif