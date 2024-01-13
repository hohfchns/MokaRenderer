#pragma once
#include <string>
#include <glm/glm.hpp>

namespace moka::renderer::wrappers
{

class Shader
{
public:
  bool initialized = false;

  Shader();
  Shader(const Shader& other);
  Shader(const std::string& vertexPath, const std::string& fragmentPath, bool absolutePath=false);
  Shader& operator=(const Shader& other);
  bool CreateShaderFromString(const std::string& vertexSource, const std::string& fragmentSource);
  bool CreateShader(const std::string& vertexPath, const std::string& fragmentPath, bool absolutePath=false);
  void Use() const;
  void Uniform(const std::string& var, int value) const;
  void Uniform(const std::string& var, bool value) const;
  void Uniform(const std::string& var, float value) const;
  void Uniform(const std::string& var, const glm::mat4& value) const;
  void Uniform(const std::string& var, const glm::vec4& value) const;
  void Uniform(const std::string& var, const glm::vec3& value) const;
  void Uniform(const std::string& var, const glm::vec2& value) const;
  const int Id() const;

private:
  unsigned int _program;
};

}
