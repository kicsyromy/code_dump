#pragma once

#include <GL/glew.h>

#include <array>
#include <vector>

class Mesh
{
public:
    constexpr Mesh() noexcept = default;
    ~Mesh() noexcept;

public:
    void load(const std::vector<GLfloat> &vertices,
        const std::vector<std::uint32_t> &indices) noexcept;
    void render() const noexcept;
    void clear() noexcept;

private:
    GLuint vao_{ 0 };
    GLuint vbo_{ 0 };
    GLuint ibo_{ 0 };
    GLsizei index_count_{ 0 };
};
