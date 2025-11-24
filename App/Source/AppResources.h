#ifndef APPRESOURCES_H
#define APPRESOURCES_H

#include "raylib.h"

class AppResources {
public:
    static void Init();
    static void Shutdown();
    static Font GetFont();

private:
    static Font s_Font;
};



#endif
