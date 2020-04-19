#include "shader.hh"

#include <spdlog/spdlog.h>

#ifdef _WIN32
#include <io.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <cstdio>
#include <cstring>
#include <memory>

Shader::Shader(std::string_view source_file, GLenum shader_type) noexcept
  : shader_type_{ shader_type }
{
    std::unique_ptr<std::FILE, decltype(&std::fclose)> fp{ std::fopen(source_file.data(), "rb"),
        &std::fclose };
    if (fp == nullptr)
    {
        const auto err = errno;
        std::fprintf(stderr, "Failed to open file at %s: %s\n", source_file.data(), strerror(err));
        std::exit(EXIT_FAILURE);
    }

    const auto file_descriptor = fileno(fp.get());
#ifdef _WIN32
    const auto length = _filelengthi64(file_descriptor);
    if (length == -1)
    {
        spdlog::error("Failed to get file size!\n");
        std::exit(EXIT_FAILURE);
    }
#else
    struct stat stbuf;
    if ((fstat(file_descriptor, &stbuf) != 0) || (!S_ISREG(stbuf.st_mode)))
    {
        const auto err = errno;
        spdlog::error("Failed to get file size! {}", strerror(err));
        std::exit(EXIT_FAILURE);
    }

    const auto length = stbuf.st_size;
#endif

    shader_source_.resize(std::size_t(length + 1));
    shader_source_.back() = '\0';
    std::fread(shader_source_.data(), std::size_t(length), 1, fp.get());
}

bool Shader::compile() noexcept
{
    auto shader = glCreateShader(shader_type_);

    const GLchar *const shader_source = shader_source_.c_str();
    const auto source_size = GLint(shader_source_.size());
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
        spdlog::error("Failed to compile {} shader: {}", shader_type_, gl_program_log.data());
        return false;
    }

    shader_id_ = shader;
    return true;
}
