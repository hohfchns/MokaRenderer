#pragma once
#include "moka/ecs/core/component.h"
#include "moka/renderer/components/mesh.h"
#include "moka/renderer/wrappers/shader.h"
#include <vector>
#include <assimp/scene.h>

namespace moka::renderer::components
{

class Model : public moka::ecs::Component
{
friend class moka::ecs::ECS;
public:
  bool visible = true;

  // Model(const Model& other);
  Model& operator=(const Model& other);

  void Init(const std::string& path);
  void SetShader(const wrappers::Shader& shader);
  const wrappers::Shader& GetShader() const;
  void Draw() const;
  void Draw(const wrappers::Shader& shader) const;
  void AddMesh(moka::ecs::Entity mesh);
  void AddMesh(moka::ecs::Entity mesh, moka::ecs::Entity parent);
  const std::vector<moka::ecs::Entity>& GetMeshes() const;

protected:
  void _Init() override;
private:
  void LoadModel(const std::string& path);
  void ProcessNode(aiNode* node, const aiScene* scene, moka::ecs::Entity parent);
  moka::ecs::Entity ProcessMesh(aiMesh* mesh, const aiScene* scene);

  wrappers::Shader _shader;
  std::vector<wrappers::Texture2D> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, wrappers::TextureType typeName);

  std::vector<moka::ecs::Entity> _meshEntities;
  std::string _directory;

  std::vector<aiNode*> _processedNodes;
};

}
