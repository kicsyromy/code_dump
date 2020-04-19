#pragma once

#include <GL/glew.h>

#include <string>
#include <string_view>

class Shader
{
public:
    Shader(std::string_view source_file, GLenum shader_type) noexcept;

public:
    bool compile() noexcept;

private:
    bool attach_to(GLuint program) const noexcept
    {
        glAttachShader(program, shader_id_);
        return true;
    }

private:
    std::string shader_source_{};
    GLenum shader_type_{};
    GLuint shader_id_{ 0 };

private:
    friend class Program;
};
