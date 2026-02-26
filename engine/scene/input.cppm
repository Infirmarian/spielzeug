module;

export module spz.engine:input;

import spz.platform;

namespace spz::engine
{
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
export class InputState
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

  [[nodiscard]] constexpr MouseCoords get_mouse_coords() const
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
}  // namespace spz::engine
