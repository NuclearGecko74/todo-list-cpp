#include "AppLayer.h"

AppLayer::AppLayer()
{

}

AppLayer::~AppLayer()
{

}

void AppLayer::OnEvent(Event &event)
{
    Layer::OnEvent(event);
}

void AppLayer::OnUpdate(float ts)
{
    Layer::OnUpdate(ts);
}

void AppLayer::OnRender()
{
    Layer::OnRender();
}