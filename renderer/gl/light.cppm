module;

export module spz.renderer.gl:light;

import :mesh_basic;

namespace spz::renderer::gl
{
export class Light
{
 private:
  Mesh_Basic m_mesh;

 public:
  Light(Mesh_Basic mesh) : m_mesh{mesh} {}

  void render() { m_mesh.render(); }
};
}  // namespace spz::renderer::gl
