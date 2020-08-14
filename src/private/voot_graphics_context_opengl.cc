#ifdef __linux__

#include "voot_graphics_context.hh"

#include "core/voot_logger.hh"

#include <SDL2.h>

#include <gpu/GrDirectContext.h>
#include <gpu/gl/GrGLInterface.h>

VT_BEGIN_PRIVATE_NAMESPACE

GraphicsContext::GraphicsContext() noexcept
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        VT_LOG_FATAL("Failed to initialize SDL: {}", SDL_GetError());
    }

    platform_window_ = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>{
        SDL_CreateWindow("",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            0,
            0,
            SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN),
        &SDL_DestroyWindow
    };

    if (platform_window_ == nullptr)
    {
        VT_LOG_FATAL("Failed to create platform render window: {}", SDL_GetError());
    }

    platform_context_ = std::unique_ptr<void, decltype(&SDL_GL_DeleteContext)>{
        SDL_GL_CreateContext(platform_window_.get()),
        &SDL_GL_DeleteContext
    };

    if (platform_context_ == nullptr)
    {
        VT_LOG_FATAL("Failed to create OpenGL context: {}", SDL_GetError());
    }

    auto interface = GrGLMakeNativeInterface();
    auto gr_context = GrDirectContext::MakeGL();
    skia_context_ = std::unique_ptr<GrDirectContext>{ gr_context.release() };
    if (skia_context_ == nullptr)
    {
        VT_LOG_FATAL("Failed to create Skia graphics context");
    }
}
GraphicsContext::~GraphicsContext() noexcept
{
    SDL_Quit();
}

VT_END_PRIVATE_NAMESPACE

#endif /* __linux__ */