module;
#include <cmath>
#include <cstdint>

#include "glad.hpp"

export module spz.renderer.gl:renderer;

namespace spz::renderer::gl
{
export inline void draw_background()
{
  static float green = 0.0f;
  static float increment = 0.0001f;
  glClearColor(0.2f, std::sin(green) * 0.5f + 0.5f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  green += increment;
}
export class Renderer
{
 public:
  Renderer(const int32_t width, const int32_t height);
};
}  // namespace spz::renderer::gl
