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
    void *native_context() const noexcept { return native_context_.get(); }

private:
#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif
    std::unique_ptr<SDL_Window, void (*)(SDL_Window *)> window_{ nullptr, nullptr };
    std::unique_ptr<void, void (*)(void *)> native_context_{ nullptr, nullptr };
    std::unique_ptr<GrDirectContext> skia_context_{ nullptr };
#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif
};

VT_END_PRIVATE_NAMESPACE