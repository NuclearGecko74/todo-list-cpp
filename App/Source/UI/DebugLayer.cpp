#include "DebugLayer.h"

static bool showMessageBox;

DebugLayer::DebugLayer()
{
    showMessageBox = false;
}

DebugLayer::~DebugLayer()
{
}

void DebugLayer::OnEvent(Event &event)
{
    Layer::OnEvent(event);
}

void DebugLayer::OnUpdate(float ts)
{
    Layer::OnUpdate(ts);
}

void DebugLayer::OnRender()
{
    Layer::OnRender();

}