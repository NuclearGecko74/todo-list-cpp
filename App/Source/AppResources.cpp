#include "AppResources.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

Font AppResources::s_Font = { 0 };

void AppResources::Init()
{
    s_Font = LoadFontEx("assets/fonts/Inter-Regular.ttf", 76, 0, 0);
    SetTextureFilter(s_Font.texture, TEXTURE_FILTER_BILINEAR);

    GuiSetFont(s_Font);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
}

void AppResources::Shutdown()
{
    UnloadFont(s_Font);
}

Font AppResources::GetFont()
{
    return s_Font;
}
