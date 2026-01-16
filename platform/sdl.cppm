module;
#include <SDL3/SDL.h>

#include <optional>
#include <string>
#include <variant>
export module spz.platform;

namespace spz::platform
{
export struct QuitEvent
{
};
export struct WindowResizedEvent
{
  uint32_t width;
  uint32_t height;
};
export struct WindowSize
{
  int32_t width;
  int32_t height;
};
using Event = std::variant<QuitEvent, WindowResizedEvent>;
export class SDLWindow
{
 private:
  SDL_Window* m_window;

 public:
  explicit SDLWindow(const std::string& title);
  ~SDLWindow();

  void shutdown();
  [[nodiscard]] std::optional<Event> poll_events();
  inline void swap_buffer() { SDL_GL_SwapWindow(m_window); }
  [[nodiscard]] WindowSize get_window_size() const
  {
    int32_t w, h;
    SDL_GetWindowSize(m_window, &w, &h);
    return WindowSize{.width = w, .height = h};
  }
};
}  // namespace spz::platform
