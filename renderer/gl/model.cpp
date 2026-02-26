module;
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

module spz.renderer.gl;

using spz::renderer::gl::Mesh;
using spz::renderer::gl::Texture;
using spz::renderer::gl::TextureType;
using spz::renderer::gl::Vertex;

namespace
{
class MaterialLoader
{
 private:
  mutable std::unordered_map<std::filesystem::path, Texture> _textureCache;

 public:
  template <TextureType texType>
  [[nodiscard]] std::vector<Texture> load_material(
      const std::filesystem::path& directory, const aiMaterial& mat) const
  {
    constexpr aiTextureType aiTexType = texType == TextureType::Diffuse
                                            ? aiTextureType_DIFFUSE
                                            : aiTextureType_SPECULAR;
    std::vector<Texture> textures;
    textures.reserve(mat.GetTextureCount(aiTexType));
    for (auto i = 0u; i < mat.GetTextureCount(aiTexType); ++i)
    {
      aiString str;
      mat.GetTexture(aiTexType, i, &str);
      const auto path = directory / str.C_Str();
      Texture tex;

      auto it = _textureCache.find(path);
      if (it != _textureCache.end())
      {
        tex = it->second;
      }
      else
      {
        tex.id =
            spz::renderer::gl::load_texture_from_file(directory / str.C_Str());
        tex.type = texType;
        _textureCache.emplace(path, tex);
      }
      textures.emplace_back(tex);
    }
    return textures;
  }
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"

Mesh processMesh(const aiMesh& mesh, const aiScene& scene,
                 const MaterialLoader& materialLoader,
                 const std::filesystem::path& dir)
{
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  std::vector<Texture> textures;
  vertices.reserve(mesh.mNumVertices);

  for (auto i = 0u; i < mesh.mNumVertices; ++i)
  {
    Vertex vertex;
    auto& mVertices = mesh.mVertices[i];
    vertex.Position.x = mVertices.x;
    vertex.Position.y = mVertices.y;
    vertex.Position.z = mVertices.z;

    auto& mNormals = mesh.mNormals[i];
    vertex.Normal.x = mNormals.x;
    vertex.Normal.y = mNormals.y;
    vertex.Normal.z = mNormals.z;
    if (mesh.mTextureCoords[0])
    {
      vertex.TexCoords.x = mesh.mTextureCoords[0][i].x;
      vertex.TexCoords.y = mesh.mTextureCoords[0][i].y;
    }
    else
    {
      vertex.TexCoords = {0.0f, 0.0f};
    }
    vertices.push_back(vertex);
  }

  for (auto i = 0u; i < mesh.mNumFaces; ++i)
  {
    auto& face = mesh.mFaces[i];
    for (auto j = 0u; j < face.mNumIndices; ++j)
    {
      indices.emplace_back(face.mIndices[j]);
    }
  }
  if (mesh.mMaterialIndex < scene.mNumMaterials)
  {
    auto* material = scene.mMaterials[mesh.mMaterialIndex];
    auto diffuseMaps =
        materialLoader.load_material<TextureType::Diffuse>(dir, *material);
    auto specularMaps =
        materialLoader.load_material<TextureType::Specular>(dir, *material);
    textures.insert_range(textures.end(), std::move(diffuseMaps));
    textures.insert_range(textures.end(), std::move(specularMaps));
  }
  return Mesh(std::move(vertices), std::move(indices), std::move(textures));
}

[[maybe_unused]] void processNode(const std::filesystem::path& dir,
                                  const MaterialLoader& materialLoader,
                                  const aiNode& node, const aiScene& scene,
                                  std::vector<Mesh>& meshes)
{
  std::cout << "Loading node" << std::endl;
  for (auto i = 0u; i < node.mNumMeshes; ++i)
  {
    auto mesh = scene.mMeshes[node.mMeshes[i]];
    meshes.push_back(processMesh(*mesh, scene, materialLoader, dir));
  }
  for (auto i = 0u; i < node.mNumChildren; ++i)
  {
    processNode(dir, materialLoader, *node.mChildren[i], scene, meshes);
  }
}
#pragma clang diagnostic pop

void loadModel(const std::filesystem::path& path, std::vector<Mesh>& meshes)
{
  Assimp::Importer importer;
  MaterialLoader matLoader;
  const auto* scene = importer.ReadFile(
      path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
  if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode)
  {
    throw std::runtime_error(std::string("ASSIMP ERROR: ") +
                             std::string(importer.GetErrorString()));
  }
  const auto dir = path.parent_path();
  processNode(dir, matLoader, *scene->mRootNode, *scene, meshes);
}
}  // namespace

namespace spz::renderer::gl
{
Model::Model(std::vector<Mesh> meshes) : _meshes(std::move(meshes)) {}
Model load_model(const std::filesystem::path& path)
{
  std::unordered_map<std::filesystem::path, Texture> textureCache;
  std::vector<Mesh> meshes;
  loadModel(path, meshes);
  return Model(std::move(meshes));
}
}  // namespace spz::renderer::gl
