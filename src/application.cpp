#include "application.h"

#include <Windows.h>

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

uwpapp::Application::Application()
{
    OutputDebugStringA("+++++++++++++ Constructor\r\n");
    Suspending += ref new SuspendingEventHandler(this, &Application::OnSuspending);
}

void uwpapp::Application::OnLaunched(LaunchActivatedEventArgs^ e)
{
    OutputDebugStringA("+++++++++++++ OnLaunched\r\n");
}

void uwpapp::Application::OnSuspending(Object^ sender, SuspendingEventArgs^ e)
{
    (void)sender;
    (void)e;
}

void uwpapp::Application::OnNavigationFailed(Object ^sender, NavigationFailedEventArgs ^e)
{
    throw ref new FailureException("Failed to load Page " + e->SourcePageType.Name);
}
