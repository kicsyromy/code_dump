#include "gulp_application.h"
#include "gulp_application_p.h"

#include <cassert>

#include "gulp_platform_p.h"

using namespace gulp;

namespace
{
    Application *appInstance = nullptr;
}

ApplicationPrivate::ApplicationPrivate(int argc, char *argv[])
  : platform_(Platform::load(argc, argv))
{
}

int ApplicationPrivate::run()
{
    platform_.startMainLoop();
}

Application::Application(int argc, char *argv[])
  : priv_{ std::make_unique<ApplicationPrivate>(argc, argv) }
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
    return priv_->run();
}
