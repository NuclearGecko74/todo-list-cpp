#include "Core/Application.h"
#include "UI/AppLayer.h"

#define RAYGUI_IMPLEMENTATION

#ifndef NDEBUG
    #include "UI/DebugLayer.h"
#endif

int main()
{
    Core::ApplicationSpecification appSpec;
    appSpec.Name = "Rizz List";
    appSpec.WindowSpec.IconPath = "assets/icon.png";

    Core::Application application(appSpec);

    application.PushLayer<AppLayer>();

#ifndef NDEBUG
    application.PushLayer<DebugLayer>();
#endif

    application.Run();
}