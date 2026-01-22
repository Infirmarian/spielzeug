module;
#include <array>
#include <cstddef>

#include "glad.hpp"

export module spz.renderer.gl:mesh;

namespace spz::renderer::gl
{
export class Mesh
{
 private:
  uint32_t m_vao;
  uint32_t m_n_vertices;

 public:
  enum class IncludeTexture : bool
  {
    False,
    True
  };
  enum class IncludeNormal : bool
  {
    False,
    True
  };
  template <IncludeTexture HasTex, IncludeNormal HasNormal, std::size_t N>
  static Mesh create_mesh(const std::array<float, N>& vertices)
  {
    constexpr auto floats_per_vertex = std::invoke(
        []
        {
          int count = 3;
          if (static_cast<bool>(HasTex))
          {
            count += 2;
          }
          if (static_cast<bool>(HasNormal))
          {
            count += 3;
          }
          return count;
        });
    constexpr auto num_vertices = N / floats_per_vertex;
    uint32_t VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                 vertices.data(), GL_STATIC_DRAW);

    auto index = 0u;
    auto offsetCount = 0u;

    glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE,
                          floats_per_vertex * sizeof(float), nullptr);
    glEnableVertexAttribArray(index);

    ++index;
    offsetCount += 3;

    if constexpr (static_cast<bool>(HasNormal))
    {
      const auto offset = reinterpret_cast<void*>(offsetCount * sizeof(float));
      glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE,
                            floats_per_vertex * sizeof(float), offset);
      glEnableVertexAttribArray(index);
      ++index;
      offsetCount += 3;
    }

    if constexpr (static_cast<bool>(HasTex))
    {
      const auto offset = reinterpret_cast<void*>(offsetCount * sizeof(float));
      glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE,
                            floats_per_vertex * sizeof(float), offset);
      glEnableVertexAttribArray(index);
      ++index;
      offsetCount += 2;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return Mesh{VAO, num_vertices};
  }

  inline void render() const
  {
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<int32_t>(m_n_vertices));
    glBindVertexArray(0);
  }

 private:
  Mesh(const uint32_t vao, const uint32_t n_vertices)
      : m_vao{vao}, m_n_vertices{n_vertices}
  {
  }
};
}  // namespace spz::renderer::gl
