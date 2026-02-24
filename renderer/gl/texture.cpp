module;
#include <sstream>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-identifier"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wcast-align"
#pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wdouble-promotion"
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#pragma clang diagnostic ignored "-Wcast-qual"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#pragma clang diagnostic ignored "-Wswitch-default"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#pragma clang diagnostic pop

#include <sys/types.h>

#include <filesystem>

#include "glad.hpp"
module spz.renderer.gl;
namespace spz::renderer::gl
{
Texture::Texture(const std::filesystem::path& path)
{
  stbi_set_flip_vertically_on_load(true);
  uint32_t format = GL_RGB;
  if (path.extension() == ".png")
  {
    format = GL_RGBA;
  }
  int width, height, channels;
  auto* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
  if (data == nullptr)
  {
    std::ostringstream ss;
    ss << "Failed to load texture. Path: " << path.c_str();
    throw std::runtime_error(ss.str());
  }
  glGenTextures(1, &m_texture);
  glBindTexture(GL_TEXTURE_2D, m_texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
}
void Texture::use(const uint32_t textureId)
{
  glActiveTexture(GL_TEXTURE0 + textureId);
  glBindTexture(GL_TEXTURE_2D, m_texture);
}
Texture::~Texture() { glDeleteTextures(1, &m_texture); }
}  // namespace spz::renderer::gl
