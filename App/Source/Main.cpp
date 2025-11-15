#include "Core/Application.h"

#include "UI/AppLayer.h"

#ifndef NDEBUG
    #include "UI/DebugLayer.h"
#endif

int main()
{
    Core::ApplicationSpecification appSpec;
    appSpec.Name = "Clash Royale 2";
    appSpec.WindowSpec.Width = 1920;
    appSpec.WindowSpec.Height = 1080;
    appSpec.WindowSpec.IconPath = "assets/icon.png";

    Core::Application application(appSpec);

    application.PushLayer<AppLayer>();

#ifndef NDEBUG
    application.PushLayer<DebugLayer>();
#endif

    application.Run();
}