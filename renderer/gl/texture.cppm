module;
#include <sys/types.h>

#include <filesystem>
export module spz.renderer.gl:texture;
namespace spz::renderer::gl
{
export class TextureObj
{
 private:
  uint32_t m_texture;  // GPU texture ID

 public:
  TextureObj(const std::filesystem::path& path);
  ~TextureObj();
  void use(uint32_t textureIdx = 0);
};
export [[nodiscard]] uint32_t load_texture_from_file(
    const std::filesystem::path& path);
}  // namespace spz::renderer::gl
