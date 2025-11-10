#ifndef TODOLIST_THEME_H
#define TODOLIST_THEME_H

#include "raylib.h"

//TODO Load Theme from Settings

namespace RizzList {
    struct Theme {
        // App Color Palette
        static constexpr Color BG_Sidebar {30, 30, 47, 255};
        static constexpr Color BG_Main {200, 179, 255, 255};
        static constexpr Color Border_Panel {78, 78, 110, 255};

        static constexpr Color BG_Panel {255, 255, 255, 255};

        static constexpr  Color Text_Light {255, 255, 255, 255};

    };
}

#endif