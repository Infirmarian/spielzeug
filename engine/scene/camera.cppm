module;
#include <algorithm>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/trigonometric.hpp>

export module spz.engine:camera;
import :input;

namespace spz::engine::scene
{
export class Camera
{
 private:
  glm::vec3 _pos;
  glm::vec3 _front;
  static constexpr glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
  float m_yaw{}, m_pitch{};

 public:
  constexpr Camera(const glm::vec3& initialPos, const glm::vec3& initialFront)
      : _pos{initialPos}, _front{initialFront}
  {
  }
  constexpr void on_update(const spz::engine::InputState& inputState,
                           const float deltaTime)
  {
    _pos += _front * inputState.get_current_inputs().Y * deltaTime;
    _pos += glm::normalize(glm::cross(_front, cameraUp)) *
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
    _front = glm::normalize(direction);
  }
  [[nodiscard]] constexpr glm::mat4 get_view_matrix() const
  {
    return glm::lookAt(_pos, _pos + _front, cameraUp);
  }
  [[nodiscard]] constexpr const glm::vec3& position() const { return _pos; }
  [[maybe_unused]] [[nodiscard]] constexpr const glm::vec3& front() const
  {
    return _front;
  }
};
}  // namespace spz::engine::scene
