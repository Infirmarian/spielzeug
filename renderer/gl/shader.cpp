module;
#include <iostream>

#include "glad.hpp"

module spx.renderer.gl;
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
enum class ShaderType
{
  Vertex,
  Fragment
};
template <ShaderType ShaderT>
[[nodiscard]] uint32_t create_shader(const char* source)
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
}  // namespace

namespace spz::renderer::gl
{
Shader::Shader()
{
  m_program_id = glCreateProgram();
  const auto vertexShaderId =
      ::create_shader<ShaderType::Vertex>(kDefaultVertexShader);
  const auto fragmentShaderId =
      ::create_shader<ShaderType::Fragment>(kDefaultFragmentShader);

  glAttachShader(m_program_id, vertexShaderId);
  glAttachShader(m_program_id, fragmentShaderId);
  glLinkProgram(m_program_id);

  glCreateShader(GL_VERTEX_SHADER);
  glDeleteShader(vertexShaderId);
  glDeleteShader(fragmentShaderId);
}

void Shader::use() const { glUseProgram(m_program_id); }
Shader::~Shader() { glDeleteProgram(m_program_id); }
}  // namespace spz::renderer::gl
