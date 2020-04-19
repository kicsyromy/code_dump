#include "application.hh"
#include "window.hh"

#include "mesh.hh"
#include "program.hh"
#include "shader.hh"

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <spdlog/spdlog.h>

#include <array>
#include <cstdint>
#include <cstring>

template<typename Number> constexpr float to_radians(Number in) noexcept
{
    static_assert(std::is_integral_v<Number> || std::is_floating_point_v<Number>);
    return float(in) * float(M_PI / 180.0);
}

int main([[maybe_unused]] int argc, [[maybe_unused]] const char **argv)
{
    Application app;

    Window main_window{ 1280, 720 };

    const std::vector vertices{ -1.f, -1.f, 0.f, 0.f, -1.f, 1.f, 1.f, -1.f, 0.f, 0.f, 1.f, 0.f };
    const std::vector indices{ 0u, 3u, 1u, 1u, 3u, 2u, 2u, 3u, 0u, 0u, 1u, 2u };

    std::vector<Mesh> meshes{ {}, {} };
    meshes[0].load(vertices, indices);
    meshes[1].load(vertices, indices);

    Shader vertex_shader{ SHADER_DIR "/vertex.glsl", GL_VERTEX_SHADER };
    vertex_shader.compile();

    Shader fragment_shader{ SHADER_DIR "/fragment.glsl", GL_FRAGMENT_SHADER };
    fragment_shader.compile();

    std::vector<Program> programs;
    programs.emplace_back();
    programs[0].link(vertex_shader, fragment_shader);

    const auto [viewport_width, viewport_height] = main_window.viewport_size();
    glm::mat4 projection =
        glm::perspective(90.f, float(viewport_width) / float(viewport_height), .1f, 100.f);

    main_window.viewport_size_changed.connect(
        &projection, [](int view_width, int view_height, void *proj) noexcept {
            auto &project = *static_cast<glm::mat4 *>(proj);
            project = glm::perspective(90.f, float(view_width) / float(view_height), .1f, 100.f);
        });

    glEnable(GL_DEPTH_TEST);

    main_window.set_draw_function([&](float) noexcept {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        {
            // [[maybe_unused]] const auto prog = program_use{ programs[0] };
            programs[0].use();

            const auto projection_uniform = programs[0].uniform(Program::Uniform::ProjectionMatrix);
            const auto model_uniform = programs[0].uniform(Program::Uniform::ModelMatrix);

            glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(projection));

            glm::mat4 model{ 1.f };
            model = glm::translate(model, { 0.f, -1.f, -2.5f });
            model = glm::scale(model, { 1.f, 1.f, 1.f });
            glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            meshes[0].render();

            model = glm::translate(glm::mat4{ 1.f }, { 0.f, 1.f, -2.5f });
            model = glm::scale(model, { 1.f, 1.f, 1.f });
            glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            meshes[1].render();
        }
    });

    app.run();

    return 0;
}
