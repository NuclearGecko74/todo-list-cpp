#include "Application.h"

#include <cassert>

namespace Core {
    static Application* s_Application = nullptr;

    Application::Application(const ApplicationSpecification &specification)
        : m_Specification(specification)
    {
        s_Application = this;

        if (m_Specification.WindowSpec.Title.empty())
        {
            m_Specification.WindowSpec.Title = m_Specification.Name;
        }

        m_Window = std::make_shared<Window>(m_Specification.WindowSpec);
    }

    Application::~Application()
    {
        s_Application = nullptr;
    }

    void Application::Run()
    {
        m_Running = true;

        while (m_Running)
        {
            if (m_Window->ShouldClose())
            {
                Stop();
                break;
            }

            float timestep = GetFrameTime();

            for (const std::unique_ptr<Layer>& layer : m_LayerStack)
                layer->OnUpdate(timestep);

            BeginDrawing();
            {
                ClearBackground(RAYWHITE);

                for (const std::unique_ptr<Layer>& layer : m_LayerStack)
                    layer->OnRender();
            }
            EndDrawing();
        }
    }

    void Application::Stop()
    {
        m_Running = false;
    }

    Vector2 Application::GetFramebufferSize() const
    {
        return m_Window->GetSize();
    }

    Application& Application::Get()
    {
        assert(s_Application);
        return *s_Application;
    }

    float Application::GetAppTime()
    {
        return static_cast<float>(GetTime());
    }

}
