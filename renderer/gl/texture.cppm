module;
#include <sys/types.h>

#include <filesystem>
export module spz.renderer.gl:texture;
namespace spz::renderer::gl
{
export class Texture
{
 private:
  uint32_t m_texture;  // GPU texture ID

 public:
  Texture(const std::filesystem::path& path);
  ~Texture();
  void use(uint32_t textureIdx = 0);
};
}  // namespace spz::renderer::gl
