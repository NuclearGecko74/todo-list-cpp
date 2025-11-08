#include "Core/Application.h"

#include "AppLayer.h"

int main()
{
    Core::ApplicationSpecification appSpec;
    appSpec.Name = "Rizz List";
    appSpec.WindowSpec.Width = 1920;
    appSpec.WindowSpec.Height = 1080;

    Core::Application application(appSpec);
    application.PushLayer<AppLayer>();
    application.Run();
}