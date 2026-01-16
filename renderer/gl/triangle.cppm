module;
#include <array>
#include <cstddef>

#include "glad.hpp"

export module spx.renderer.gl:triangle;

namespace spz::renderer::gl
{
export class Triangle
{
 private:
  uint32_t m_vao;
  uint32_t m_n_vertices;

 public:
  template <std::size_t N>
  static Triangle create_mesh(const std::array<float, N>& vertices)
  {
    constexpr auto num_vertices = N / 5;
    uint32_t VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                 vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    const auto offset = reinterpret_cast<void*>(3 * sizeof(float));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), offset);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return Triangle{VAO, num_vertices};
  }

  inline void render() const
  {
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<int32_t>(m_n_vertices));
    glBindVertexArray(0);
  }

 private:
  Triangle(const uint32_t vao, const uint32_t n_vertices)
      : m_vao{vao}, m_n_vertices{n_vertices}
  {
  }
};
}  // namespace spz::renderer::gl
