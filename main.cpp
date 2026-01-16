#include <array>
#include <iostream>
#include <variant>

import spz.platform;
import spx.renderer.gl;

template <class... Ts>
struct overloaded : Ts...
{
  using Ts::operator()...;
};

int main()
{
  spz::platform::SDLWindow window("Spielzeug");
  const auto window_size = window.get_window_size();
  spz::renderer::gl::Renderer renderer(window_size.width, window_size.height);

  spz::renderer::gl::Triangle triangle =
      spz::renderer::gl::Triangle::create_mesh(
          std::to_array<float>({// positions        // texture coords
                                -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  // bottom left
                                0.5f, -0.5f, 0.0f, 1.0f, 0.0f,   // bottom right
                                0.0f, 0.5f, 0.0f, 0.5f, 1.0f}));  // top
  auto shader = spz::renderer::gl::Shader();

  bool running = true;
  while (running)
  {
    while (auto event = window.poll_events())
    {
      std::visit(
          overloaded{[&running](spz::platform::QuitEvent) { running = false; },
                     [](spz::platform::WindowResizedEvent e)
                     {
                       std::cout << "Window resized to " << e.width << "x"
                                 << e.height << "\n";
                     }},
          *event);
    }
    spz::renderer::gl::draw_background();
    shader.use();
    triangle.render();
    window.swap_buffer();
  }
}
