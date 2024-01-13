#pragma once
#include "moka/renderer/components/mesh.h"
#include <unordered_map>
#include "glm/fwd.hpp"
#include "moka/ecs/core/component_ptr.hpp"
#include "moka/world/components/transform.h"
#include "moka/renderer/types/material.h"
#include <glm/glm.hpp>


namespace moka::renderer::components
{

class InstancedMesh : public Mesh
{
public:
  moka::renderer::types::Material material;

  void Draw() const;

  void Init(std::vector<types::Vertex>& vertices,
    std::vector<unsigned int>& indices,
    std::vector<wrappers::Texture2D>& textures,
    const std::string& vertexShader,
    const std::string& fragmentShader
  );

  void Init(std::vector<types::Vertex>& vertices,
    std::vector<unsigned int>& indices,
    std::vector<wrappers::Texture2D>& textures) override;

  void SetupBuffers();

  moka::ecs::Entity AddNewTransform();
  void AddTransform(moka::ecs::Entity transform);

  virtual void Draw(const wrappers::Shader& shader) const override { throw moka::not_implemented(); }

  void Refresh() const;

private:
  bool _initialized = false;
  unsigned int _transformsVBO;
  size_t _max_instances;

  std::vector<moka::ecs::ComponentPtr<moka::world::components::Transform>> _transforms;
  wrappers::Shader _shader;

  std::string _vertexShader;
  std::string _fragmentShader;
};

}

