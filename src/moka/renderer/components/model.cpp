#include "moka/renderer/components/model.h"
#include "moka/renderer/components/camera.h"
#include "moka/renderer/components/mesh.h"
#include "moka/world/components/name.h"
#include "moka/world/components/transform.h"
#include "moka/assets/resource_dir.h"
#include "moka/ecs/core/ecs.hpp"
#include "moka/ecs/core/system.hpp"
#include "moka/renderer/main_loop.h"
#include <assimp/metadata.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <filesystem>
#include <glm/fwd.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iterator>
#include <stdexcept>

using std::vector;

using namespace moka::ecs;
using namespace moka::world::components;
using namespace moka::renderer::wrappers;
using namespace moka::renderer::types;

namespace moka::renderer::components
{

Model& Model::operator=(const Model& other)
{
  Entity E_fakeEntity = ECS::Get().newEntity();
  ECS::Get().AddComponent<Transform>(E_fakeEntity);
  // TODO free all meshes first
  for (Entity E_mesh : _meshEntities)
  {
    ECS::Get().GetComponentP<Transform>(E_mesh)->SetParent(E_fakeEntity);
    ECS::Get().GetComponentP<Mesh>(E_mesh)->visible = false;
  }

  GetComponentP<Transform>()->SetChildren(std::vector<Entity>());
  _meshEntities.clear();

  visible = other.visible;
  _directory = other._directory;
  SetShader(other.GetShader());

  for (int E_mesh : other._meshEntities)
  {
    if (!ECS::Get().GetComponentP<Mesh>(E_mesh))
    {
      continue;
    }

    Entity E_newMesh = ECS::Get().newEntity();
    ECS::Get().AddComponent<Transform>(E_newMesh);
    ECS::Get().AddComponent<Mesh>(E_newMesh);
    auto C_newMesh = ECS::Get().GetComponentP<Mesh>(E_newMesh);
    auto C_mesh = ECS::Get().GetComponentP<Mesh>(E_mesh);
    auto vertices = C_mesh->_vertices;
    auto indices = C_mesh->_indices;
    auto textures = C_mesh->_textures;
    C_newMesh->Init(vertices, indices, textures);
    AddMesh(E_newMesh);
  }

  return *this;
}

/*Model::Model(const Model& other)
{
  // TODO free all meshed first
  _meshEntities.clear();
  visible = other.visible;
  _directory = other._directory;

  for (int E_mesh : other._meshEntities)
  {
    Mesh* C_mesh = ECS::Get().GetComponentP<Mesh>(E_mesh);
    if (C_mesh == nullptr)
    {
      continue;
    }

    Entity E_newMesh = ECS::Get().newEntity();
    ECS::Get().AddComponent<Mesh>(E_newMesh);
    ECS::Get().AddComponent<Transform>(E_newMesh);
    Mesh* C_newMesh = ECS::Get().GetComponentP<Mesh>(E_newMesh);
    C_newMesh->Init(C_mesh->_vertices, C_mesh->_indices, C_mesh->_textures);
    AddMesh(E_newMesh);
  }

  return *this;
}*/

void Model::_Init()
{
  if (!GetComponentP<Transform>())
  {
    ECS::Get().AddComponent<Transform>(GetEntity());
  }
}

void Model::Init(const std::string& path)
{
  LoadModel(path);
}

void Model::Draw() const
{
  if (!GetShader().initialized)
  {
    throw std::logic_error("Cannot use Draw() on Model without a set Shader.");
  }

  Draw(GetShader());
}

void Model::Draw(const Shader& shader) const
{
  if (!visible)
  {
    return;
  }

  for (Entity mesh : _meshEntities)
  {
    ECS::Get().GetComponentP<Mesh>(mesh)->Draw(shader);
  }
}

void Model::SetShader(const wrappers::Shader& shader)
{
  _shader = shader;
}

const wrappers::Shader& Model::GetShader() const
{
  return _shader;
}

void Model::AddMesh(Entity mesh)
{
  AddMesh(mesh, _ownerEntity);
}

void Model::AddMesh(Entity mesh, Entity parent)
{
  GetComponent<Transform>().AddChild(mesh);
  _meshEntities.push_back(mesh);
}

const std::vector<Entity>& Model::GetMeshes() const
{
  return _meshEntities;
}

void Model::LoadModel(const std::string& path)
{
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(
      moka::assets::relativePath(path), 
      aiProcess_Triangulate | aiProcess_FlipUVs
      );

  if (!scene
      || scene->mFlags 
      & AI_SCENE_FLAGS_INCOMPLETE
      || !scene->mRootNode)
  {
    std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
    return;
  }

  _processedNodes.clear();
  std::cout << "Starting model/scene processing" << std::endl;
  // _directory = path.substr(0, path.find_last_of('/'));
  _directory = std::filesystem::path(path).parent_path();
  ProcessNode(scene->mRootNode, scene, GetEntity());
  std::cout << "Finished model/scene processing" << std::endl;
  _processedNodes.clear();
}

void Model::ProcessNode(aiNode *node, const aiScene *scene, Entity parent)
{
  // Processed this node already
  if (std::find(_processedNodes.begin(), _processedNodes.end(), node)
      != _processedNodes.end())
  {
    std::cout << "Skipping node " << node->mName.C_Str() << std::endl;
    return;
  }

  std::cout << "Processing node " << node->mName.C_Str() << std::endl;
  // Parent was not yet processed
  if (
      std::find(
      _processedNodes.begin(), _processedNodes.end(),
      node->mParent
      ) == _processedNodes.end()
      && node->mParent != nullptr
  )
  {
    std::cout << "Processing parent " << node->mParent->mName.C_Str() << std::endl;
    ProcessNode(node->mParent, scene, parent);
    parent = *(ECS::Get().GetComponentP<Transform>(parent)->GetChildren().end() - 1);
  }

  Entity nodeEntity = ECS::Get().newEntity();
  ECS::Get().AddComponent<NameComponent>(nodeEntity);
  ECS::Get().GetComponentP<NameComponent>(nodeEntity)->name = node->mName.C_Str();
  ECS::Get().AddComponent<Transform>(nodeEntity);
  ECS::Get().GetComponentP<Transform>(nodeEntity)->SetParent(parent);

  std::vector<Entity> children;
  // process all the node’s meshes (if any)
  for(unsigned int i = 0; i < node->mNumMeshes; i++)
  {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    Entity meshEntity = ProcessMesh(mesh, scene);
    AddMesh(meshEntity, nodeEntity);
    children.push_back(meshEntity);
  }

  auto trans = ECS::Get().GetComponentP<Transform>(nodeEntity);
  if (trans)
  {
    aiVector3t<float> position, scaling;
    aiQuaterniont<float> rotation;
    // std::cout << glm::to_string(rotDeg) << std::endl;
    node->mTransformation.Decompose(scaling, rotation, position);

    glm::vec3 rotDeg(
      glm::degrees(rotation.x * 2.f),
      glm::degrees(rotation.y * 2.f),
      glm::degrees(rotation.z * 2.f)
    );

    trans->SetRotation(rotDeg);

    if (trans->GetParent())
    {
      trans->SetPosition(
        glm::vec3(
          position.x - position.x / 2, position.y - position.y / 2, position.z - position.z / 2
        )
      );
    }
    else
    {
      trans->SetPosition(
        glm::vec3(
          position.x, position.y, position.z
        )
      );
    }
    

    trans->SetScale(
      glm::vec3(
        scaling.x, scaling.y, scaling.z
      )
    );

    std::cout << "name: " << node->mName.C_Str() << std::endl;
    auto name = ECS::Get().GetComponentP<NameComponent>(trans->GetEntity());
    if (name)
    {
      std::cout << "Entity name: " << name->name << std::endl;
    }
    aiNode* parent = node->mParent;
    if (parent != nullptr)
    {
      std::cout << "parent name: " << parent->mName.C_Str() << std::endl;
      auto parentName = ECS::Get().GetComponentP<NameComponent>(trans->GetParent());
      if (parentName)
      {
        std::cout << "Parent Entity name: " << parentName->name << std::endl;
      }
      std::cout << "pos: " << glm::to_string(trans->Position()) << std::endl;
      std::cout << "global pos: " << glm::to_string(trans->GlobalPosition()) << std::endl;
      auto parent = ECS::Get().GetComponentP<Transform>(trans->GetParent());
      if (parent)
      {
        glm::vec3 parentPos = parent->Position();
        std::cout << "parent pos: " << glm::to_string(parentPos) << std::endl;
        glm::vec3 parentPosGlob = parent->GlobalPosition();
        std::cout << "parent global pos: " << glm::to_string(parentPosGlob) << std::endl;
      }
    }
    else
    {
      std::cout << "No parent, continuing" << std::endl;
    }
  }

  std::cout << "Finished Processing node " << node->mName.C_Str() << std::endl;
  aiMetadata* md = node->mMetaData;
  if (md != nullptr)
  {
    std::cout << "Processing node " << node->mName.C_Str() << " MetaData" << std::endl;
    for (size_t i = 0; i < md->mNumProperties; i++)
    {
      const aiString* key;
      const aiMetadataEntry* entry;
      md->Get(i, key, entry);
      if (std::string(key->C_Str()) == "components")
      {
        if (entry->mType != aiMetadataType::AI_AISTRING)
        {
          continue;
        }

        std::string strData = std::string(
            (*(aiString*)
             entry->mData
            ).C_Str()
          );

        std::istringstream tmp(strData);
        std::string token;
        while (getline(tmp, token, ','))
        {
          // std::string component = token;
          // component.erase(std::remove_if(component.begin(), component.end(), isspace), component.end());
          //
          // Entity entity = *(_meshEntities.end() - 1);
          // ECS::Get().AddComponent(entity, component);
        }
      }
      else if (std::string(key->C_Str()) == "flags")
      {
        if (entry->mType != aiMetadataType::AI_AISTRING)
        {
          continue;
        }

        std::string strData = std::string(
            (*(aiString*)
             entry->mData
            ).C_Str()
          );

        std::istringstream tmp(strData);
        std::string token;
        while (getline(tmp, token, ','))
        {
          std::string flag = token;
          flag.erase(std::remove_if(flag.begin(), flag.end(), isspace), flag.end());

          if (flag == "nomesh")
          {
            for (auto child : children)
            {
              ECS::Get().GetComponentP<Mesh>(child)->visible = false;
            }
          }
        }
      }
      else if (std::string(key->C_Str()) == "system")
      {
        if (entry->mType != aiMetadataType::AI_AISTRING)
        {
          continue;
        }

        std::string systemId = std::string(
            (*(aiString*)
             entry->mData
            ).C_Str()
          );

        systemId.erase(std::remove_if(systemId.begin(), systemId.end(), isspace), systemId.end());
      }
    }
  }
  
  std::cout << "Finished Processing node " << node->mName.C_Str() << " MetaData" << std::endl;

  _processedNodes.push_back(node);

  // then do the same for each of its children
  for(unsigned int i = 0; i < node->mNumChildren; i++)
  {
    ProcessNode(node->mChildren[i], scene, nodeEntity);
  }
}

Entity Model::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
  vector<Vertex> vertices;
  vector<unsigned int> indices;
  vector<Texture2D> textures;

  for(unsigned int i = 0; i < mesh->mNumVertices; i++)
  {
    Vertex vertex;
    vertex.position = 
      glm::vec3(mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z
              );

    vertex.normal = 
    glm::vec3(mesh->mNormals[i].x,
              mesh->mNormals[i].y,
              mesh->mNormals[i].z
            );

    if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
    {
       vertex.texCoords = glm::vec2 (
          mesh->mTextureCoords[0][i].x,
          mesh->mTextureCoords[0][i].y
       );
    }
    else
    {
      vertex.texCoords = glm::vec2(0.0f, 0.0f);
    }

    vertices.push_back(vertex);
  }

  // process indices
  for(unsigned int i = 0; i < mesh->mNumFaces; i++)
  {
    aiFace face = mesh->mFaces[i];
    for(unsigned int j = 0; j < face.mNumIndices; j++)
    indices.push_back(face.mIndices[j]);
  }

  // process material
  if(mesh->mMaterialIndex >= 0)
  {
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    vector<Texture2D> diffuseMaps = LoadMaterialTextures(
        material,
        aiTextureType_DIFFUSE, 
        TextureType::DIFFUSE);

    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    vector<Texture2D> specularMaps = LoadMaterialTextures(
        material,
        aiTextureType_SPECULAR,
        TextureType::DIFFUSE);

    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
  }

  for(int i = textures.size(); i >= 0 && i < 3; i++)
  {
    std::string filename = "resources/texture_default" + std::to_string(i + 1) + ".png";
    Texture2D t;
    t.Load(filename);
    textures.push_back(std::move(t));
  }

  Entity newMeshEntity = ECS::Get().newEntity();
  ECS::Get().AddComponent<Mesh>(newMeshEntity);
  ECS::Get().AddComponent<Transform>(newMeshEntity);
  auto newMesh = ECS::Get().GetComponentP<Mesh>(newMeshEntity);
  newMesh->Init(
    vertices,
    indices,
    textures
  );

  return newMeshEntity;
}

vector<Texture2D> Model::LoadMaterialTextures(aiMaterial *mat, aiTextureType type, TextureType typeName)
{
  vector<Texture2D> textures;
  for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
  {
    aiString str;
    mat->GetTexture(type, i, &str);
    Texture2D texture;
    std::filesystem::path fullPath = _directory + "/" + str.C_Str();
    texture.Load(fullPath);
    texture.type = typeName;
    textures.push_back(texture);
  }

  return textures;
}



}


