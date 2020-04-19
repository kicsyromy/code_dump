#include <cstdint>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <spdlog/spdlog.h>

constexpr GLint WIDTH{ 800 };
constexpr GLint HEIGHT{ 600 };

int main([[maybe_unused]] int argc, [[maybe_unused]] const char **argv)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        spdlog::error("Failed to initialize SDL: {}", SDL_GetError());
        std::exit(EXIT_FAILURE);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    std::unique_ptr<SDL_Window, void (*)(SDL_Window *)> main_window_ptr{
        SDL_CreateWindow(APPLICATION_NAME,
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            WIDTH,
            HEIGHT,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE),
        &SDL_DestroyWindow
    };
    auto *main_window = main_window_ptr.get();
    if (main_window == nullptr)
    {
        spdlog::error("Failed to create window: {}", SDL_GetError());
        std::exit(EXIT_FAILURE);
    }

    std::unique_ptr<void, decltype(&SDL_GL_DeleteContext)> gl_context_ptr{
        SDL_GL_CreateContext(main_window), &SDL_GL_DeleteContext
    };
    [[maybe_unused]] auto *gl_context = gl_context_ptr.get();

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        spdlog::error("Failed to initialize GLEW");
        std::exit(EXIT_FAILURE);
    }

    int viewport_width;
    int viewport_height;
    SDL_GL_GetDrawableSize(main_window, &viewport_width, &viewport_height);

    glViewport(0, 0, viewport_width, viewport_height);

    auto start = std::chrono::high_resolution_clock::now();
    bool quit = false;
    SDL_Event event;
    while (!quit)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT) { quit = true; }
            else
                switch (event.type)
                {
                case SDL_KEYDOWN:
                    break;
                }
        }

        [[maybe_unused]] const auto elapsed =
            float(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - start)
                      .count()) /
            1000.f;
        start = std::chrono::high_resolution_clock::now();

        glClearColor(1.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(main_window);
    }

    return 0;
}
