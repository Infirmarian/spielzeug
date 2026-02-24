module;
#include <cstddef>
#include <cstdint>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <vector>

#include "glad.hpp"

export module spz.renderer.gl:mesh;
import :shader;

namespace spz::renderer::gl
{
struct Vertex
{
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoords;
};

enum class TextureType
{
  Unknown = 0,
  Diffuse = 1,
  Specular = 2
};

struct Texture
{
  TextureType type;
  uint32_t id;
};

class Mesh
{
 private:
  std::vector<Vertex> _vertices;
  std::vector<uint32_t> _indices;
  std::vector<Texture> _textures;

 public:
  Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices,
       std::vector<Texture> textures)
      : _vertices(std::move(vertices)),
        _indices{std::move(indices)},
        _textures(std::move(textures))
  {
    setupMesh();
  }
  void render(Shader& shader)
  {
    char diffuse_name[] = "material.texture_diffuse0";
    char specular_name[] = "material.texture_specular0";
    uint8_t diffuseCount = 0, specularCount = 0;
    for (auto i = 0u; i < _textures.size(); ++i)
    {
      glActiveTexture(GL_TEXTURE0 + i);
      const char* name;
      if (_textures[i].type == TextureType::Diffuse)
      {
        diffuse_name[sizeof(diffuse_name) - 2] = '0' + (diffuseCount++);
        name = diffuse_name;
      }
      else if (_textures[i].type == TextureType::Specular)
      {
        specular_name[sizeof(specular_name) - 2] = '0' + (specularCount++);
        name = specular_name;
      }
      else
      {
        // Error
        continue;
      }
      shader.set_int(name, static_cast<int32_t>(i));
      glBindTexture(GL_TEXTURE_2D, _textures[i].id);
    }
    glActiveTexture(GL_TEXTURE9);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<int32_t>(_indices.size()),
                   GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
  }

 private:
  uint32_t VAO, VBO, EBO;
  [[maybe_unused]] uint32_t _padding;
  void setupMesh()
  {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<int64_t>(_vertices.size() * sizeof(Vertex)),
                 &_vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<int64_t>(_indices.size() * sizeof(uint32_t)),
                 &_indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, Normal)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));

    glBindVertexArray(0);
  }
};
}  // namespace spz::renderer::gl
