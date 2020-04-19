#include "program.hh"

#include <spdlog/spdlog.h>

Program::Program() noexcept
  : program_id_{ glCreateProgram() }
{
    if (!program_id_)
    {
        spdlog::error("Failed to create shader program %s", glewGetString(glGetError()));
        std::exit(EXIT_FAILURE);
    }
}

Program::~Program() noexcept { clear(); }

GLint Program::uniform(Program::Uniform uniform) const noexcept
{
    switch (uniform)
    {
    case Uniform::ProjectionMatrix:
        return glGetUniformLocation(program_id_, "projection");
    case Uniform::ModelMatrix:
        return glGetUniformLocation(program_id_, "model");
    }

    return -1;
}

void Program::clear() noexcept
{
    if (program_id_ != 0) { glDeleteProgram(program_id_); }
}

bool Program::link_program() noexcept
{
    GLint result{ -1 };
    std::array<GLchar, 1024> gl_program_log;

    glLinkProgram(program_id_);
    glGetProgramiv(program_id_, GL_LINK_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(
            program_id_, gl_program_log.size() * sizeof(GLchar), nullptr, gl_program_log.data());
        gl_program_log.back() = '\0';
        spdlog::error("Failed to link program: {}", gl_program_log.data());
        return false;
    }

    glValidateProgram(program_id_);
    glGetProgramiv(program_id_, GL_VALIDATE_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(
            program_id_, gl_program_log.size() * sizeof(GLchar), nullptr, gl_program_log.data());
        gl_program_log.back() = '\0';
        spdlog::error("Failed to validate program: {}", gl_program_log.data());
        return false;
    }

    return true;
}
