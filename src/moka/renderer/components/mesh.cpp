#include "moka/renderer/components/mesh.h"
#include "moka/renderer/components/camera.h"
#include "moka/renderer/main_loop.h"
#include "moka/renderer/renderer.h"
#include "moka/renderer/wrappers/texture.h"
#include "moka/world/components/transform.h"
#include <string>

using std::vector;
using std::string;

using namespace moka::ecs;
using namespace moka::renderer::types;
using namespace moka::renderer::wrappers;

namespace moka::renderer::components
{

void Mesh::Init(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<Texture2D>& textures)
{
  _vertices = std::move(vertices);
  _indices = std::move(indices);
  _textures = std::move(textures);

  SetupMesh();
}

void Mesh::SetupMesh()
{
  glGenVertexArrays(1, &_vao);
  glGenBuffers(1, &_vbo);
  glGenBuffers(1, &_ebo);

  glBindVertexArray(_vao);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), &_indices[0], GL_STATIC_DRAW);

  // vertex positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
  
  // vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
  
  // vertex texture coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

  glBindVertexArray(0);
}

void Mesh::Draw(const Shader& shader) const
{
  if (!visible)
  {
    return;
  }

  shader.Use();

  glm::mat4 view = ECS::Get().GetComponentP<Camera>(misc::Singleton<MainLoop>::Ref().mainCamera)->View();
  shader.Uniform("view", view);

  shader.Uniform("color_modulate", material.modulate);

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

    shader.Uniform(TextureTypeStr[(int)name] + number, (int)i);
    _textures[i].Bind();
  }

  auto trans = GetComponentP<moka::world::components::Transform>();
  if (trans)
  {
    shader.Uniform("model", trans->Model());
  }

  glActiveTexture(GL_TEXTURE0);
  
  // draw mesh
  glBindVertexArray(_vao);
  glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

const std::vector<types::Vertex>& Mesh::GetVertices() const
{
  return _vertices;
}

const std::vector<unsigned int>& Mesh::GetIndices() const
{
  return _indices;
}

const std::vector<wrappers::Texture2D>& Mesh::GetTextures() const
{
  return _textures;
}

}
