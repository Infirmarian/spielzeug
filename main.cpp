#include <array>
#include <chrono>
#include <filesystem>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <variant>

import spz.platform;
import spz.renderer.gl;

template <class... Ts>
struct overloaded : Ts...
{
  using Ts::operator()...;
};

namespace
{
constexpr auto unit_cube = std::to_array<float>(
    {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
     0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
     -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

     -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

     -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
     -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
     -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
     0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
     0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
     0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
     -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

     -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f});

const std::filesystem::path kTextureDir() { return "renderer/gl/textures/"; };
const std::filesystem::path kShaderDir() { return "renderer/gl/shaders/"; };
}  // namespace

int main()
{
  spz::platform::SDLWindow window("Spielzeug");
  const auto window_size = window.get_window_size();
  spz::renderer::gl::Renderer renderer(window_size.width, window_size.height);

  spz::renderer::gl::Triangle triangle =
      spz::renderer::gl::Triangle::create_mesh(unit_cube);
  const auto tex1 = spz::renderer::gl::Texture(kTextureDir() / "wall.jpg");
  const auto vertexShader = kShaderDir() / "shader.vert";
  const auto fragmentShader = kShaderDir() / "shader.frag";
  glm::mat4 projection =
      glm::perspective(glm::radians(45.0f),
                       static_cast<float>(window_size.width) /
                           static_cast<float>(window_size.height),
                       0.1f, 100.0f);
  const auto view =
      glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
  auto shader = spz::renderer::gl::Shader(vertexShader, fragmentShader);

  bool running = true;
  const auto time = std::chrono::steady_clock::now();
  while (running)
  {
    float elapsed_seconds =
        std::chrono::duration<float>(std::chrono::steady_clock::now() - time)
            .count();
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
    auto pos =
        glm::rotate(glm::mat4(1.0f), glm::radians(elapsed_seconds * 20.0f),
                    glm::vec3(1.0f, 1.0f, 0.0f));
    pos = glm::scale(pos, glm::vec3(1.0f, 1.0f, 1.0f) *
                              (std::sin(elapsed_seconds * 2.0f) * 0.5f + 1.0f));
    shader.set_mat4("projection", projection);
    shader.set_mat4("view", view);
    shader.set_mat4("model", pos);
    shader.use();
    triangle.render();
    window.swap_buffer();
  }
}
