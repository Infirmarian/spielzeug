import spz.platform;

int main()
{
  spz::platform::SDLWindow window("Spielzeug");
  while (true)
  {
    while (auto event = window.poll_events())
    {
      if (event.has_value())
      {
        return 0;
      }
    }
    window.swap_buffer();
  }
}
