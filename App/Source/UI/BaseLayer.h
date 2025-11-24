#ifndef BASELAYER_H
#define BASELAYER_H

#include "Core/Layer.h"
#include "../AppResources.h"

class BaseLayer : public Core::Layer {
protected:
    void DrawLabel(const char* text, int x, int y, int size, Color color)
    {
        Font font = AppResources::GetFont();
        DrawTextEx(font, text, { (float)x, (float)y }, (float)size, 1.0f, color);
    }
};

#endif
