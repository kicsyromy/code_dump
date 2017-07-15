#include "gulp_platform_loader_p.h"

#include <cstdlib>
#include <iostream>

#include <unistd.h>
#include <dlfcn.h>

using namespace gulp;

namespace
{
    using LibraryHandle = void *;

    LibraryHandle openLibrary(const char *path)
    {
        auto handle = dlopen(path, RTLD_LAZY);
        if (handle == nullptr)
        {
            std::cerr << "Failed to open platform library. Error was " << dlerror() << '\n';
            std::exit(EXIT_FAILURE);
        }

        return handle;
    }

    void closeLibrary(LibraryHandle libraryHandle)
    {
        dlclose(libraryHandle);
    }

    template <typename Signature>
    Signature loadFunction(LibraryHandle libraryHandle, const char *functionName)
    {
        Signature function = (Signature)dlsym(libraryHandle, functionName);
        const char *error = dlerror();
        if (error != nullptr)
        {
            std::cerr << "Failed to find \"" << functionName << "\" symbol. Error was: " << error << '\n';
            std::exit(EXIT_FAILURE);
        }

        return function;
    }
}

PlatformLoader::PlatformLoader(int argc, char *argv[])
    : handle_(openLibrary("/home/kicsyromy/workspace/build-GuLP-Desktop_Qt_5_8_0_GCC_x86_64-Debug/src/platform/gtk/libplatform_gtk.so.0.0.1"))
    , loadedPlatform_(loadFunction<Platform *(*)(int, char *[])>(handle_, "loadPlatform")(argc, argv))
{
}

PlatformLoader::~PlatformLoader()
{
    if (handle_ != nullptr)
    {
        closeLibrary(handle_);
    }
}
