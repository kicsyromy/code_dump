#pragma once

#include "shader.hh"

#include <GL/glew.h>

#include <string_view>

class Program
{
public:
    enum class Uniform
    {
        ProjectionMatrix,
        ModelMatrix
    };

public:
    Program() noexcept;
    ~Program() noexcept;

public:
    template<typename... Shaders> bool link(const Shaders &... shader) noexcept
    {
        static_assert(std::is_same_v<std::common_type_t<Shaders...>, Shader>);
        (shader.attach_to(program_id_) && ...);

        return link_program();
    }

    GLint uniform(Uniform uniform) const noexcept;
    void use() const noexcept { glUseProgram(program_id_); }
    void clear() noexcept;

private:
    bool link_program() noexcept;

private:
    GLuint program_id_{ 0 };
};

struct program_use
{
    program_use(const Program &program) noexcept { program.use(); }
    ~program_use() noexcept { glUseProgram(0); }
};
