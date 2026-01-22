module;

export module spz.renderer.gl:light;

import :mesh;

namespace spz::renderer::gl
{
export class Light
{
 private:
  Mesh m_mesh;

 public:
  Light(Mesh mesh) : m_mesh{mesh} {}

  void render() { m_mesh.render(); }
};
}  // namespace spz::renderer::gl
