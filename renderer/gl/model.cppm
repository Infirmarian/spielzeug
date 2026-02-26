module;
#include <cstddef>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <vector>
#include <filesystem>

export module spz.renderer.gl:model;
import :shader;
import :mesh;

namespace spz::renderer::gl
{
export class Model
{
 private:
  std::vector<Mesh> _meshes;

 public:
  Model(std::vector<Mesh> meshes);
  void render(Shader& shader)
  {
    for (auto& mesh : _meshes)
    {
      mesh.render(shader);
    }
  }
};
export Model load_model(const std::filesystem::path& path);
}  // namespace spz::renderer::gl
