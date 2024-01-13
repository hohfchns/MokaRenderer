#pragma once
#include "moka/ecs/core/component.h"
#include "moka/renderer/types/material.h"
#include "moka/renderer/types/vertex.h"
#include "moka/renderer/wrappers/shader.h"
#include "moka/renderer/wrappers/texture.h"
#include <vector>

namespace moka::physics::components
{
  class MeshCollider;
  class TerrainCollider;
}

namespace moka::renderer::components
{

class Mesh : public moka::ecs::Component
{
  friend class Model;
  friend class InstancedMesh;
  friend class moka::physics::components::MeshCollider;
  friend class moka::physics::components::TerrainCollider;
public:
  virtual void Init(std::vector<types::Vertex>& vertices,
    std::vector<unsigned int>& indices,
    std::vector<wrappers::Texture2D>& textures);

  virtual void Draw(const wrappers::Shader& shader) const;

  const std::vector<types::Vertex>& GetVertices() const;
  const std::vector<unsigned int>& GetIndices() const;
  const std::vector<wrappers::Texture2D>& GetTextures() const;

  moka::renderer::types::Material material;

protected:
  std::vector<types::Vertex> _vertices;
  std::vector<unsigned int> _indices;
  std::vector<wrappers::Texture2D> _textures;

  bool visible = true;

protected:
  unsigned int _vao, _vbo, _ebo;

  virtual void SetupMesh();
};

}
