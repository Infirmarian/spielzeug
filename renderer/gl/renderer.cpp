module;
#include "glad.hpp"
module spz.renderer.gl;

namespace spz::renderer::gl
{
Renderer::Renderer(const int32_t width, const int32_t height)
{
  gladLoadGL();
  glViewport(0, 0, width, height);
}
}  // namespace spz::renderer::gl
