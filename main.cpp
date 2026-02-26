#include <chrono>
#include <filesystem>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/trigonometric.hpp>
#include <print>
#include <variant>

#include "tools/tracy.hpp"

import spz.platform;
import spz.renderer.gl;
import spz.engine;

template <class... Ts>
struct overloaded : Ts...
{
  using Ts::operator()...;
};

namespace
{
const std::filesystem::path kShaderDir() { return "renderer/gl/shaders/"; };
const std::filesystem::path kModelDirectory() { return "renderer/gl/models/"; };
}  // namespace

int main()
{
  spz::platform::SDLWindow window("Spielzeug");
  const auto window_size = window.get_window_size();
  spz::renderer::gl::Renderer renderer(window_size.width, window_size.height);
  auto basicShader = spz::renderer::gl::Shader(kShaderDir() / "basic.vert",
                                               kShaderDir() / "basic.frag");

  auto backpack =
      spz::renderer::gl::load_model(kModelDirectory() / "backpack" / "backpack.obj");

  glm::mat4 projection =
      glm::perspective(glm::radians(45.0f),
                       static_cast<float>(window_size.width) /
                           static_cast<float>(window_size.height),
                       0.1f, 100.0f);
  bool running = true;
  const auto time = std::chrono::steady_clock::now();
  spz::engine::InputState inputs;
  spz::engine::scene::Camera cam{{-5.0f, 1.0f, 0.f}, {0.1f, 0.0f, -0.1f}};
  float last_frame_elapsed_seconds{0.0f};
  while (running)
  {
    TRACE_FRAME();
    float elapsed_seconds =
        std::chrono::duration<float>(std::chrono::steady_clock::now() - time)
            .count();

    const float delta_time = elapsed_seconds - last_frame_elapsed_seconds;
    last_frame_elapsed_seconds = elapsed_seconds;

    while (auto event = window.poll_events())
    {
      std::visit(
          overloaded{
              [&running](spz::platform::QuitEvent) { running = false; },
              [](spz::platform::WindowResizedEvent e)
              { std::println("Window resized to {}x{}", e.width, e.height); },
              [&inputs](spz::platform::KeyDownEvent e)
              {
                inputs.record_key_down(e.Key);
                std::println("Pressed key {}", static_cast<int32_t>(e.Key));
              },
              [&inputs](spz::platform::KeyUpEvent e)
              {
                inputs.record_key_up(e.Key);
                std::println("Released key {}", static_cast<int32_t>(e.Key));
              },
              [&inputs](spz::platform::MouseMotionEvent e)
              { inputs.record_mouse_movement(e); },
          },
          *event);
    }
    spz::renderer::gl::draw_background();
    cam.on_update(inputs, delta_time);
    const auto cameraMat = cam.get_view_matrix();

    inputs.record_mouse_movement({0.f, 0.f});

    basicShader.use();
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.f, 0.f, 0.f));
    model = glm::scale(model, glm::vec3(1.f));
    basicShader.set_mat4("model", model);
    basicShader.set_mat4("projection", projection);
    basicShader.set_mat4("view", cameraMat);
    backpack.render(basicShader);

    window.swap_buffer();
  }
}
