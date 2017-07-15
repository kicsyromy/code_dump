#include "gulp_platform_p.h"

#include <string>
#include <iostream>

#include "gulp_platform_loader_p.h"

using namespace gulp;

namespace
{
    PlatformLoader *platform = nullptr;
}

Platform &Platform::load(int argc, char *argv[])
{
    static PlatformLoader p(argc, argv);
    platform = &p;

    return p.get();
}

Platform::TypeInitializers Platform::getInitializers()
{
    static auto &&initializers = platform->get().initializers();
    return initializers;
}
