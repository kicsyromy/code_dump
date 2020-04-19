#include "mesh.hh"

Mesh::~Mesh() noexcept { clear(); }

void Mesh::load(const std::vector<GLfloat> &vertices, const std::vector<uint32_t> &indices) noexcept
{
    index_count_ = decltype(index_count_)(indices.size());
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &ibo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        GLsizeiptr(indices.size() * sizeof(std::decay_t<decltype(indices)>::value_type)),
        indices.data(),
        GL_STATIC_DRAW);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER,
        GLsizeiptr(vertices.size() * sizeof(std::decay_t<decltype(vertices)>::value_type)),
        vertices.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void Mesh::render() const noexcept
{
    if (vao_ == 0 || vbo_ == 0 || ibo_ == 0 || index_count_ == 0) return;

    glBindVertexArray(vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
    glDrawElements(GL_TRIANGLES, index_count_, GL_UNSIGNED_INT, nullptr);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::clear() noexcept
{
    if (ibo_ != 0)
    {
        glDeleteBuffers(1, &ibo_);
        ibo_ = 0;
    }

    if (vbo_ != 0)
    {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }

    if (vao_ != 0)
    {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }

    index_count_ = 0;
}
