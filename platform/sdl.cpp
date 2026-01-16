module;
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_opengl.h>

#include <optional>
#include <stdexcept>

module spz.platform;

namespace
{
spz::platform::KeyCode translate_keycode(uint32_t sdlKeyCode)
{
  if (sdlKeyCode >= SDLK_A && sdlKeyCode <= SDLK_Z)
  {
    return static_cast<spz::platform::KeyCode>(
        sdlKeyCode - SDLK_A + static_cast<uint32_t>(spz::platform::KeyCode::A));
  }
  return spz::platform::KeyCode::Unknown;
}
}  // namespace

namespace spz::platform
{
SDLWindow::SDLWindow(const std::string& title)
{
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
    throw std::runtime_error("SDL could not initialize!");
  }

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  m_window = SDL_CreateWindow(title.c_str(), 800, 600,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  if (!m_window)
  {
    SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
    throw std::runtime_error("Window could not be created!");
  }
  SDL_WarpMouseInWindow(m_window, 400, 300);
  SDL_SetWindowRelativeMouseMode(m_window, true);

  SDL_GLContext context = SDL_GL_CreateContext(m_window);
  if (!context)
  {
    SDL_Log("OpenGL context could not be created! SDL error: %s\n",
            SDL_GetError());
    throw std::runtime_error("OpenGL context could not be created!");
  }
}
SDLWindow::~SDLWindow() { shutdown(); }
void SDLWindow::shutdown()
{
  if (m_window)
  {
    SDL_DestroyWindow(m_window);
  }
  m_window = nullptr;
  SDL_Quit();
}

std::optional<Event> SDLWindow::poll_events()
{
  if (SDL_Event event; SDL_PollEvent(&event))
  {
    if (event.type == SDL_EVENT_QUIT)
    {
      return QuitEvent{};
    }
    if (event.type == SDL_EVENT_WINDOW_RESIZED)
    {
      return WindowResizedEvent{
          .width = static_cast<uint32_t>(event.window.data1),
          .height = static_cast<uint32_t>(event.window.data2)};
    }
    if (event.type == SDL_EVENT_KEY_DOWN)
    {
      if (event.key.key == SDLK_ESCAPE)
      {
        return QuitEvent{};
      }
      return KeyDownEvent{translate_keycode(event.key.key)};
    }
    if (event.type == SDL_EVENT_KEY_UP)
    {
      return KeyUpEvent{translate_keycode(event.key.key)};
    }
    if (event.type == SDL_EVENT_MOUSE_MOTION)
    {
      return MouseMotionEvent{event.motion.xrel, event.motion.yrel};
    }
  }
  return std::nullopt;
}
#pragma endregion Public
}  // namespace spz::platform
