#pragma once

#include "voot_global.hh"

#include <memory>

using SDL_Window = struct SDL_Window;

class GrDirectContext;

VT_BEGIN_PRIVATE_NAMESPACE

class VOOT_API GraphicsContext
{
public:
    GraphicsContext() noexcept;
    ~GraphicsContext() noexcept;

public:
    GrDirectContext &skia_context() const noexcept { return *skia_context_; };
    void *platform_context() const noexcept { return platform_context_.get(); }

private:
#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif
    std::unique_ptr<SDL_Window, void (*)(SDL_Window *)> platform_window_{ nullptr, nullptr };
    std::unique_ptr<void, void (*)(void *)> platform_context_{ nullptr, nullptr };
    std::unique_ptr<GrDirectContext> skia_context_{ nullptr };
#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif
};

VT_END_PRIVATE_NAMESPACE