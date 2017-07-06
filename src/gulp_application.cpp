#include "gulp_application.h"
#include "gulp_application_p.h"

#include <cassert>

using namespace gulp;

namespace
{
    Application *appInstance = nullptr;
}

Application::Application(int argc, char *argv[])
  : priv_{ std::make_unique<ApplicationPrivate>() }
{
    appInstance = this;
}

Application::~Application()
{
    appInstance = nullptr;
}

Application &Application::instance()
{
    assert(appInstance != nullptr);
    return *appInstance;
}

int Application::run()
{
    // start event loop
    return 0;
}
