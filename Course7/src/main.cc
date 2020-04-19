#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <spdlog/spdlog.h>

#include <array>
#include <cstdint>
#include <cstring>

constexpr GLint WIDTH{ 800 };
constexpr GLint HEIGHT{ 600 };

constexpr auto VERTEX_SHADER{
    R"(#version 330

layout (location = 0) in vec3 pos;

out vec4 vertexColor;

uniform mat4 model;

void main()
{
    gl_Position = model * vec4(pos, 1.0);
    vertexColor = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);
})"
};

constexpr auto FRAGMENT_SHADER{
    R"(#version 330

in vec4 vertexColor;

out vec4 color;

void main()
{
    color = vertexColor;
})"
};

template<typename Number> constexpr float to_radians(Number in) noexcept
{
    static_assert(std::is_integral_v<Number> || std::is_floating_point_v<Number>);
    return float(in) * float(M_PI / 180.0);
}

std::tuple<GLuint, GLuint, GLuint> create_triangle() noexcept
{
    constexpr std::array vertices{ -1.f, -1.f, 0.f, 0.f, -1.f, 1.f, 1.f, -1.f, 0.f, 0.f, 1.f, 0.f };
    constexpr std::array indices{ 0u, 3u, 1u, 1u, 3u, 2u, 2u, 3u, 0u, 0u, 1u, 2u };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(decltype(indices)::value_type),
        indices.data(),
        GL_STATIC_DRAW);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(decltype(vertices)::value_type),
        vertices.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    return { vao, vbo, ibo };
}

void add_shader(GLuint program, std::string_view shader_code, GLenum shader_type) noexcept
{
    auto shader = glCreateShader(shader_type);

    const GLchar *const shader_source = shader_code.data();
    const auto source_size = GLint(shader_code.size());
    glShaderSource(shader, 1, &shader_source, &source_size);

    GLint result{ 0 };
    std::array<GLchar, 1024> gl_program_log;

    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(
            shader, gl_program_log.size() * sizeof(GLchar), nullptr, gl_program_log.data());
        gl_program_log.back() = '\0';
        spdlog::error("Failed to compile {} shader: {}", shader_type, gl_program_log.data());
        std::exit(EXIT_FAILURE);
    }

    glAttachShader(program, shader);
}

std::pair<GLuint, GLint> compile_shaders() noexcept
{
    auto program = glCreateProgram();
    if (!program)
    {
        spdlog::error("Failed to create shader program %s", glewGetString(glGetError()));
        std::exit(EXIT_FAILURE);
    }

    add_shader(program, VERTEX_SHADER, GL_VERTEX_SHADER);
    add_shader(program, FRAGMENT_SHADER, GL_FRAGMENT_SHADER);

    GLint result{ 0 };
    std::array<GLchar, 1024> gl_program_log;

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(
            program, gl_program_log.size() * sizeof(GLchar), nullptr, gl_program_log.data());
        gl_program_log.back() = '\0';
        spdlog::error("Failed to link program: {}", gl_program_log.data());
        std::exit(EXIT_FAILURE);
    }

    glValidateProgram(program);
    glGetProgramiv(program, GL_VALIDATE_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(
            program, gl_program_log.size() * sizeof(GLchar), nullptr, gl_program_log.data());
        gl_program_log.back() = '\0';
        spdlog::error("Failed to validate program: {}", gl_program_log.data());
        std::exit(EXIT_FAILURE);
    }

    auto model_uniform = glGetUniformLocation(program, "model");

    return { program, model_uniform };
}

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
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
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

    glEnable(GL_DEPTH_TEST);

    int viewport_width;
    int viewport_height;
    SDL_GL_GetDrawableSize(main_window, &viewport_width, &viewport_height);
    glViewport(0, 0, viewport_width, viewport_height);

    auto [vao, vbo, ibo] = create_triangle();
    auto [program, model_uniform] = compile_shaders();

    bool direction{ true };
    float triangle_offset{ 0 };
    float triangle_max_offset{ .7f };
    float triangle_increment{ 1.5f };

    float rotation_angle{ 0.f };

    bool size_direction{ true };
    float size{ .4f };
    float max_size{ .8f };
    float min_size{ .1f };

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
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                    {
                        SDL_GL_GetDrawableSize(main_window, &viewport_width, &viewport_height);
                        glViewport(0, 0, viewport_width, viewport_height);
                    }
                    break;
                }
        }

        [[maybe_unused]] const auto elapsed =
            float(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - start)
                      .count()) /
            1000.f;
        start = std::chrono::high_resolution_clock::now();

        if (direction)
            triangle_offset += triangle_increment * elapsed;
        else
            triangle_offset -= triangle_increment * elapsed;
        if (std::abs(triangle_offset) >= triangle_max_offset) { direction = !direction; }

        rotation_angle += 45.f * elapsed;
        if (rotation_angle > 360.f) { rotation_angle -= 360.f; }

        if (size_direction)
            size += 1.0f * elapsed;
        else
            size -= 1.0f * elapsed;
        if (std::abs(size) >= max_size || std::abs(size) <= min_size)
        { size_direction = !size_direction; }

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        glm::mat4 model{ 1.f };
        model = glm::rotate(model, to_radians(rotation_angle), { 1.f, 1.f, 1.f });
        // model = glm::translate(model, { triangle_offset, 0.f, 0.f });
        model = glm::scale(model, { .4f, 0.4f, 1.f });
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glDrawElements(GL_TRIANGLES, 12 /* indices.size() */, GL_UNSIGNED_INT, nullptr);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);

        SDL_GL_SwapWindow(main_window);
    }

    return 0;
}
