#include <iostream>
#include <variant>

import spz.platform;

template <class... Ts>
struct overloaded : Ts...
{
  using Ts::operator()...;
};

int main()
{
  spz::platform::SDLWindow window("Spielzeug");
  bool running = true;
  while (running)
  {
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
    window.swap_buffer();
  }
}
