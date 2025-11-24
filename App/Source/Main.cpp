#define RAYGUI_IMPLEMENTATION

#include "AppResources.h"
#include "Core/Application.h"
#include "UI/AppLayer.h"

#ifndef NDEBUG
    #include "UI/DebugLayer.h"
#endif

int main()
{
    Core::ApplicationSpecification appSpec;
    appSpec.Name = "Rizz List";
    appSpec.WindowSpec.IconPath = "assets/icon.png";

    Core::Application application(appSpec);

    AppResources::Init();

    application.PushLayer<AppLayer>();

#ifndef NDEBUG
    application.PushLayer<DebugLayer>();
#endif

    application.Run();

    AppResources::Shutdown();
}