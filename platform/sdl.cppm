module;
#include <SDL3/SDL.h>

#include <optional>
#include <string>
export module spz.platform;

namespace spz::platform
{
struct QuitEvent
{
};
export class SDLWindow
{
 private:
  SDL_Window* m_window;

 public:
  explicit SDLWindow(const std::string& title);
  ~SDLWindow();

  void shutdown();
  [[nodiscard]] std::optional<QuitEvent> poll_events();
  inline void swap_buffer() { SDL_GL_SwapWindow(m_window); }
};
}  // namespace spz::platform
