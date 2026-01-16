#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <optional>
#include <print>
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

constexpr auto cubePositions = std::to_array<glm::vec3>(
    {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f, 5.0f, -15.0f),
     glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
     glm::vec3(2.4f, -0.4f, -3.5f), glm::vec3(-1.7f, 3.0f, -7.5f),
     glm::vec3(1.3f, -2.0f, -2.5f), glm::vec3(1.5f, 2.0f, -2.5f),
     glm::vec3(1.5f, 0.2f, -1.5f), glm::vec3(-1.3f, 1.0f, -1.5f)});
const std::filesystem::path kTextureDir() { return "renderer/gl/textures/"; };
const std::filesystem::path kShaderDir() { return "renderer/gl/shaders/"; };

struct InputAxis
{
  float X;
  float Y;
};
struct MouseCoords
{
  // Relative to the last mouse movement
  float XRel;
  float YRel;
};
class InputStateHolder
{
 private:
  MouseCoords m_mouse_coords{};
  bool w_down{};
  bool a_down{};
  bool s_down{};
  bool d_down{};

 public:
  [[nodiscard]] constexpr InputAxis get_current_inputs() const
  {
    float y = 1.f * w_down + -1.f * s_down;
    float x = 1.f * d_down + -1.f * a_down;
    return InputAxis{x, y};
  }

  [[maybe_unused]] [[nodiscard]] constexpr MouseCoords get_mouse_coords() const
  {
    return m_mouse_coords;
  }

  constexpr void record_mouse_movement(
      const spz::platform::MouseMotionEvent& motion)
  {
    m_mouse_coords.XRel = motion.XRel;
    m_mouse_coords.YRel = motion.YRel;
  }

  constexpr void record_key_down(const spz::platform::KeyCode keyCode)
  {
    record_key<true>(keyCode);
  }

  constexpr void record_key_up(const spz::platform::KeyCode keyCode)
  {
    record_key<false>(keyCode);
  }

 private:
  template <bool KeyDown>
  constexpr void record_key(const spz::platform::KeyCode keyCode)
  {
    switch (keyCode)
    {
      case spz::platform::KeyCode::A:
        a_down = KeyDown;
        break;
      case spz::platform::KeyCode::D:
        d_down = KeyDown;
        break;
      case spz::platform::KeyCode::W:
        w_down = KeyDown;
        break;
      case spz::platform::KeyCode::S:
        s_down = KeyDown;
        break;
      default:
        break;
    }
  }
};

class Camera
{
 private:
  glm::vec3 m_pos;
  glm::vec3 m_front;
  static constexpr glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
  float m_yaw{}, m_pitch{};

 public:
  constexpr Camera(const glm::vec3& initialPos, const glm::vec3& initialFront)
      : m_pos{initialPos}, m_front{initialFront}
  {
  }
  constexpr void on_update(const InputStateHolder& inputState,
                           const float deltaTime)
  {
    m_pos += m_front * inputState.get_current_inputs().Y * deltaTime;
    m_pos += glm::normalize(glm::cross(m_front, cameraUp)) *
             inputState.get_current_inputs().X * deltaTime;

    constexpr float sensitivity = 0.1f;
    const auto xOffset = sensitivity * inputState.get_mouse_coords().XRel;
    const auto yOffset = sensitivity * inputState.get_mouse_coords().YRel;

    m_yaw += xOffset;
    m_pitch -= yOffset;

    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

    glm::vec3 direction;
    direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    direction.y = sin(glm::radians(m_pitch));
    direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(direction);
  }
  [[nodiscard]] constexpr glm::mat4 get_view_matrix() const
  {
    return glm::lookAt(m_pos, m_pos + m_front, cameraUp);
  }
};
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
  auto shader = spz::renderer::gl::Shader(vertexShader, fragmentShader);

  bool running = true;
  const auto time = std::chrono::steady_clock::now();
  InputStateHolder inputs;
  Camera cam{{0.0f, 0.0f, -3.0f}, {0.0f, 0.0f, 1.0f}};
  float last_frame_elapsed_seconds{0.0f};
  while (running)
  {
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
              {
                std::cout << "Window resized to " << e.width << "x" << e.height
                          << "\n";
              },
              [&inputs](spz::platform::KeyDownEvent e)
              {
                inputs.record_key_down(e.Key);
                std::cout << "Pressed key: " << static_cast<int32_t>(e.Key)
                          << std::endl;
              },
              [&inputs](spz::platform::KeyUpEvent e)
              {
                inputs.record_key_up(e.Key);
                std::cout << "Released key: " << static_cast<int32_t>(e.Key)
                          << std::endl;
              },
              [&inputs](spz::platform::MouseMotionEvent e)
              {
                std::println("Mouse at {:f}x{:f}", e.XRel, e.YRel);
                inputs.record_mouse_movement(e);
              },
          },
          *event);
    }
    spz::renderer::gl::draw_background();
    cam.on_update(inputs, delta_time);
    const auto cameraMat = cam.get_view_matrix();

    inputs.record_mouse_movement({0.f, 0.f});

    for (auto posVec : cubePositions)
    {
      auto pos = glm::mat4(1.0f);
      pos = glm::translate(pos, posVec);
      pos = glm::rotate(pos, glm::radians(elapsed_seconds * 20.0f),
                        glm::vec3(1.0f, 1.0f, 0.0f));
      shader.set_mat4("model", pos);
      triangle.render();
    }
    shader.set_mat4("projection", projection);
    shader.set_mat4("view", cameraMat);
    shader.use();
    window.swap_buffer();
  }
}
