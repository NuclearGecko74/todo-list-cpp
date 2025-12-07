#ifndef TODOLIST_THEME_H
#define TODOLIST_THEME_H

#include "raylib.h"

// TODO Load Theme from Settings

struct Theme {
    // App Color Palette
    static constexpr Color BG_Sidebar {30, 30, 47, 255};
    static constexpr Color BG_Main {200, 179, 255, 255};
    static constexpr Color Border_Panel {110, 110, 172, 255};
    static constexpr Color BG_Panel {255, 255, 255, 255};

    static constexpr  Color Text_Light {255, 255, 255, 255};
    static constexpr Color Text_Dark {0, 0, 0, 255};

    static constexpr Color Text_Muted {150, 150, 150, 255};

    static constexpr Color Accent_Red { 255, 80, 80, 255 };
    static constexpr Color Accent_Blue { 80, 200, 255, 255 };
    static constexpr Color Selection_Blue { 200, 200, 255, 255 };
};

#endif