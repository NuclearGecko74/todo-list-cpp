#include "Window.h"

#include <iostream>

namespace Core {
    Window::Window(const WindowSpecification &specification)
        : m_Specification(specification)
    {
        uint32_t flags = 0;
        if (m_Specification.IsResizeable)
            flags |= FLAG_WINDOW_RESIZABLE;
        if (m_Specification.VSync)
            flags |= FLAG_VSYNC_HINT;

        if (flags != 0)
            SetConfigFlags(flags);

        InitWindow(m_Specification.Width, m_Specification.Height, m_Specification.Title.c_str());

        SetTargetFPS(60);

        std::cout << "Raylib window initialized.\n";
    }

    Window::~Window()
    {
        CloseWindow();
        std::cout << "Raylib window closed.\n";
    }

    Vector2 Window::GetSize()
    {
        return { static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) };
    }

    bool Window::ShouldClose() const
    {
        return WindowShouldClose();
    }
}