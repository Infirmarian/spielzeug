#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>
#include <format>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/trigonometric.hpp>
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
constexpr auto unit_cube_textured_normal = std::to_array<float>(
    {-0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.5f,  -0.5f,
     -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,
     0.0f,  -1.0f, 1.0f,  1.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
     1.0f,  1.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  1.0f,
     -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,

     -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.5f,  -0.5f,
     0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,
     0.0f,  1.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     1.0f,  1.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
     -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

     -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,
     -0.5f, -1.0f, 0.0f,  0.0f,  1.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f,
     0.0f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
     0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f,  0.0f,
     -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,
     -0.5f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,
     0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
     0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

     -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.5f,  -0.5f,
     -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,
     -1.0f, 0.0f,  1.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
     1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.0f,
     -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,

     -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
     -0.5f, 0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,
     1.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
     -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f});

[[maybe_unused]] const auto unit_cube = std::invoke(
    []
    {
      std::array<float, (3 * unit_cube_textured_normal.size()) / 8> result;
      for (uint32_t i = 0, j = 0; i < unit_cube_textured_normal.size(); ++i)
      {
        if (i % 8 < 3)
        {
          result[j] = unit_cube_textured_normal[i];
          ++j;
        }
      }
      return result;
    });

[[maybe_unused]] constexpr auto cubePositions = std::to_array<glm::vec3>(
    {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f, 5.0f, -15.0f),
     glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
     glm::vec3(2.4f, -0.4f, -3.5f), glm::vec3(-1.7f, 3.0f, -7.5f),
     glm::vec3(1.3f, -2.0f, -2.5f), glm::vec3(1.5f, 2.0f, -2.5f),
     glm::vec3(1.5f, 0.2f, -1.5f), glm::vec3(-1.3f, 1.0f, -1.5f)});
const std::filesystem::path kTextureDir() { return "renderer/gl/textures/"; };
const std::filesystem::path kShaderDir() { return "renderer/gl/shaders/"; };
const std::filesystem::path kModelDirectory() { return "renderer/gl/models/"; };

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
    direction.x = cosf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch));
    direction.y = sinf(glm::radians(m_pitch));
    direction.z = sinf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch));
    m_front = glm::normalize(direction);
  }
  [[nodiscard]] constexpr glm::mat4 get_view_matrix() const
  {
    return glm::lookAt(m_pos, m_pos + m_front, cameraUp);
  }
  [[nodiscard]] constexpr const glm::vec3& position() const { return m_pos; }
  [[maybe_unused]] [[nodiscard]] constexpr const glm::vec3& front() const
  {
    return m_front;
  }
};
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

  auto unitCubeBlank = spz::renderer::gl::Mesh_Basic::create_mesh<
      spz::renderer::gl::Mesh_Basic::IncludeTexture::False,
      spz::renderer::gl::Mesh_Basic::IncludeNormal::False>(unit_cube);
  auto light = spz::renderer::gl::Light{unitCubeBlank};
  auto tex1 = spz::renderer::gl::TextureObj(kTextureDir() / "container2.png");
  auto tex2 =
      spz::renderer::gl::TextureObj(kTextureDir() / "container2_specular.png");
  const auto vertexShader = kShaderDir() / "shader.vert";
  const auto fragmentShader = kShaderDir() / "shader.frag";
  glm::mat4 projection =
      glm::perspective(glm::radians(45.0f),
                       static_cast<float>(window_size.width) /
                           static_cast<float>(window_size.height),
                       0.1f, 100.0f);
  auto shader = spz::renderer::gl::Shader(vertexShader, fragmentShader);
  auto lightShader =
      spz::renderer::gl::Shader(vertexShader, kShaderDir() / "light.frag");

  bool running = true;
  const auto time = std::chrono::steady_clock::now();
  InputStateHolder inputs;
  Camera cam{{-5.0f, 1.0f, 0.f}, {0.1f, 0.0f, -0.1f}};
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

    shader.use();
    shader.set_int("material.diffuse", 0);
    shader.set_int("material.specular", 1);
    tex1.use(0);
    tex2.use(1);
    shader.set_float("material.shininess", 32.f);

    shader.set_vec3("dirLight.ambient", glm::vec3(0.1f));
    shader.set_vec3("dirLight.diffuse", glm::vec3(0.8f));
    shader.set_vec3("dirLight.specular", glm::vec3(1.f));
    shader.set_vec3("dirLight.direction", {-0.2f, -1.f, -.3f});

    shader.set_vec3("viewPos", cam.position());

    std::array<glm::vec3, 4> pointLightPositions = {
        glm::vec3(0.7f, 0.2f, 2.0f), glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f), glm::vec3(0.0f, 0.0f, -3.0f)};
    for (uint32_t i = 0; i < pointLightPositions.size(); ++i)
    {
      shader.set_float(std::format("pointLights[{}].constant", i).c_str(), 1.f);
      shader.set_float(std::format("pointLights[{}].linear", i).c_str(), 0.09f);
      shader.set_float(std::format("pointLights[{}].quadratic", i).c_str(),
                       0.032f);
      shader.set_vec3(std::format("pointLights[{}].position", i).c_str(),
                      pointLightPositions[i]);
      shader.set_vec3(std::format("pointLights[{}].ambient", i).c_str(),
                      glm::vec3(0.05f));
      shader.set_vec3(std::format("pointLights[{}].diffuse", i).c_str(),
                      glm::vec3(0.8f));
      shader.set_vec3(std::format("pointLights[{}].specular", i).c_str(),
                      glm::vec3(1.f));
    }

    for (float angle = 0.f; const auto& cubePos : cubePositions)
    {
      auto pos = glm::mat4(1.0f);
      pos = glm::translate(pos, cubePos);
      pos = glm::rotate(pos, glm::radians(elapsed_seconds * 25.f + angle),
                        {1.0f, 0.5f, 1.0f});
      angle += 20.f;
      shader.set_mat4("model", pos);
      // unitCubeObj.render();
    }

    shader.set_mat4("projection", projection);
    shader.set_mat4("view", cameraMat);

    basicShader.use();
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.f, 0.f, 0.f));
    model = glm::scale(model, glm::vec3(1.f));
    basicShader.set_mat4("model", model);
    basicShader.set_mat4("projection", projection);
    basicShader.set_mat4("view", cameraMat);
    backpack.render(basicShader);

    lightShader.use();
    for (auto lightPos : pointLightPositions)
    {
      auto posMat = glm::translate(glm::mat4{1.f}, lightPos);
      posMat = glm::scale(posMat, glm::vec3{0.2f});
      lightShader.set_mat4("model", posMat);
      light.render();
    }
    lightShader.set_mat4("projection", projection);
    lightShader.set_mat4("view", cameraMat);
    window.swap_buffer();
  }
}
