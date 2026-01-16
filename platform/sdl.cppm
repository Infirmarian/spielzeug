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
};
}  // namespace spz::platform
