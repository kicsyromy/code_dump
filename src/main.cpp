#include "application.h"

#include <Windows.h>

using namespace Platform;
using namespace Windows::UI::Xaml;

int main(Array<String^>^ args)
{
    OutputDebugStringA("+++++++++++++ main\r\n");

    Application::Start(ref new ApplicationInitializationCallback([](ApplicationInitializationCallbackParams^ p) {
        uwpapp::Application^ app(ref new uwpapp::Application());
    }));
}
