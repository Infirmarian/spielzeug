module;

#include <filesystem>
#include <glm/fwd.hpp>

export module spz.renderer.gl:shader;

namespace spz::renderer::gl
{
export class Shader
{
 private:
  uint32_t m_program_id;

 public:
  Shader();  // Default shader with pink color for debugging
  Shader(const std::filesystem::path& vertexShader,
         const std::filesystem::path& fragmentShader);
  ~Shader();
  /// @brief Activate the shader
  void use() const;
  void set_bool(const char* name, bool value);
  void set_float(const char* name, float value);
  void set_int(const char* name, int32_t value);
  void set_mat4(const char* name, const glm::mat4& mat);
};
}  // namespace spz::renderer::gl
