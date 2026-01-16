module;
#include <filesystem>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>
#include <iostream>

#include "glad.hpp"

module spz.renderer.gl;
namespace
{
constexpr auto kDefaultVertexShader = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec2 aTexCoord;

        void main()
        {
            gl_Position = vec4(aPos, 1.0);
        }
)";
constexpr auto kDefaultFragmentShader = R"(
        #version 330 core
        out vec4 FragColor;

        void main()
        {
            FragColor = vec4(1.0, 0.0, 1.0, 1.0); // Pink color for debugging
        }
)";
[[nodiscard]] std::string load_shader_source(const std::filesystem::path& path)
{
  std::ifstream file(path);
  if (!file.is_open())
  {
    throw std::runtime_error("Failed to open shader file: " + path.string());
  }
  return std::string((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
}
enum class ShaderType
{
  Vertex,
  Fragment
};
template <ShaderType ShaderT>
[[nodiscard]] uint32_t create_single_shader(const char* source)
{
  constexpr auto glShaderType =
      ShaderT == ShaderType::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
  const auto shader = glCreateShader(glShaderType);
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);
  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    std::array<char, 2048> infoLog;
    glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog.data());
    std::cout << "ERROR::COMPILATION_FAILED\n"
              << infoLog.data() << "\nShader Source: " << source << std::endl;
    throw std::runtime_error("Shader compilation failed");
  }
  return shader;
}

/// @brief Create a shader with vertex and fragment shaders
[[nodiscard]] uint32_t create_shader(const char* vertexShaderSrc,
                                     const char* fragmentShaderSrc)
{
  auto programId = glCreateProgram();
  const auto vertexShaderId =
      create_single_shader<ShaderType::Vertex>(vertexShaderSrc);
  const auto fragmentShaderId =
      create_single_shader<ShaderType::Fragment>(fragmentShaderSrc);

  glAttachShader(programId, vertexShaderId);
  glAttachShader(programId, fragmentShaderId);
  glLinkProgram(programId);

  glCreateShader(GL_VERTEX_SHADER);
  glDeleteShader(vertexShaderId);
  glDeleteShader(fragmentShaderId);
  return programId;
}
}  // namespace

namespace spz::renderer::gl
{
Shader::Shader()
{
  m_program_id = create_shader(kDefaultVertexShader, kDefaultFragmentShader);
}

Shader::Shader(const std::filesystem::path& vertexShaderPath,
               const std::filesystem::path& fragmentShaderPath)
{
  const auto vertexShaderSrc = load_shader_source(vertexShaderPath);
  const auto fragmentShaderSrc = load_shader_source(fragmentShaderPath);
  m_program_id =
      create_shader(vertexShaderSrc.c_str(), fragmentShaderSrc.c_str());
}

void Shader::set_mat4(const char* name, const glm::mat4& mat)
{
  glUniformMatrix4fv(glGetUniformLocation(m_program_id, name), 1, GL_FALSE,
                     glm::value_ptr(mat));
}

void Shader::use() const { glUseProgram(m_program_id); }
Shader::~Shader() { glDeleteProgram(m_program_id); }
}  // namespace spz::renderer::gl
