#include "moka/renderer/components/instanced_mesh.h"
#include "moka/ecs/core/defines.h"
#include "moka/renderer/components/camera.h"
#include "moka/renderer/main_loop.h"
#include "moka/renderer/renderer.h"
#include "moka/world/components/transform.h"
#include <glm/gtx/string_cast.hpp>
#include <stdexcept>
#include <glm/glm.hpp>

using std::string;

using namespace moka::ecs;
using namespace moka::renderer::types;
using namespace moka::renderer::wrappers;

namespace moka::renderer::components
{
void InstancedMesh::Draw() const 
{
  if (!visible)
  {
    return;
  }
  if (!_initialized)
  {
    return;
  }

  _shader.Use();

  glm::mat4 view = ECS::Get().GetComponentP<Camera>(misc::Singleton<MainLoop>::Ref().mainCamera)->View();
  _shader.Uniform("view", view);

  _shader.Uniform("color_modulate", material.modulate);

  unsigned int diffuseNr = 1;
  unsigned int specularNr = 1;
  for(unsigned int i = 0; i < _textures.size(); i++)
  {
    glActiveTexture(GL_TEXTURE0 + i); // activate texture unit first
    // retrieve texture number (the N in diffuse_textureN)
    string number;
    TextureType name = _textures[i].type;
    if(name == TextureType::DIFFUSE)
    {
      number = std::to_string(diffuseNr++);
    }
    else if(name == TextureType::SPECULAR)
    {
      number = std::to_string(specularNr++);
    }

    _shader.Uniform(TextureTypeStr[(int)name] + number, (int)i);
    glActiveTexture(GL_TEXTURE0);
    _textures[i].Bind();
  }

  Refresh();

  glBindVertexArray(_vao);
  glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(_indices.size()), GL_UNSIGNED_INT, 0, static_cast<unsigned int>(_transforms.size()));
  glBindVertexArray(0);
}


void InstancedMesh::SetupBuffers()
{
  glGenBuffers(1, &_transformsVBO);

  Refresh();

  glBindVertexArray(_vao);

  glEnableVertexAttribArray(3); 
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
  glEnableVertexAttribArray(4); 
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(1 * sizeof(glm::vec4)));
  glEnableVertexAttribArray(5); 
  glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
  glEnableVertexAttribArray(6); 
  glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

  glVertexAttribDivisor(3, 1);
  glVertexAttribDivisor(4, 1);
  glVertexAttribDivisor(5, 1);
  glVertexAttribDivisor(6, 1);

  glBindVertexArray(0);
}

void InstancedMesh::Init(std::vector<types::Vertex>& vertices,
  std::vector<unsigned int>& indices,
  std::vector<wrappers::Texture2D>& textures)
{
  Init(vertices, indices, textures, "resources/base_instanced.vertex.glsl", "resources/base_material.fragment.glsl");
}

void InstancedMesh::Init(std::vector<types::Vertex>& vertices,
    std::vector<unsigned int>& indices,
    std::vector<wrappers::Texture2D>& textures,
    const std::string& vertexShader,
    const std::string& fragmentShader
  )
{
  Mesh::Init(vertices, indices, textures);

  _initialized = true;
  _vertexShader = vertexShader;
  _fragmentShader = fragmentShader;

  _shader = Shader(vertexShader, fragmentShader);

  SetupBuffers();
}

Entity InstancedMesh::AddNewTransform()
{
  Entity E_transform = ECS::Get().newEntity();
  ECS::Get().AddComponent<moka::world::components::Transform>(E_transform);
  AddTransform(E_transform);
  return E_transform;
}

void InstancedMesh::AddTransform(Entity transform)
{
  auto C_transform = ECS::Get().GetComponentP<moka::world::components::Transform>(transform);
  _transforms.push_back(C_transform);

  Refresh();
}

void InstancedMesh::Refresh() const
{
  glm::mat4 models[_transforms.size()];
  for (size_t i = 0; i < _transforms.size(); i++)
  {
    models[i] = (_transforms[i]->Model());
  }

  glBindBuffer(GL_ARRAY_BUFFER, _transformsVBO);
  glBufferData(GL_ARRAY_BUFFER, _transforms.size() * sizeof(glm::mat4), &models[0], GL_STATIC_DRAW);
}

}
