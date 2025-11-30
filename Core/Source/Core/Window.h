#ifndef TODOLIST_WINDOW_H
#define TODOLIST_WINDOW_H

#include <string>

#include "raylib.h"

namespace Core {
    struct WindowSpecification {
        std::string Title;
        uint32_t Width = 1280;
        uint32_t Height = 720;
        bool IsResizeable = true;
        bool VSync = true;
        std::string IconPath;
        bool StartMaximized = true;
    };

    class Window {
    public:
        Window(const WindowSpecification& specification = WindowSpecification());
        ~Window();

        Vector2 GetSize();

        bool ShouldClose() const;
    private:
        WindowSpecification m_Specification;
    };
}

#endif