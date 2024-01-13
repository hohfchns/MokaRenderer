#include <glm/gtc/type_ptr.hpp>
#include "moka/renderer/main_loop.h"
#include "moka/renderer/wrappers/shader.h"
#include "moka/renderer/renderer.h"
#include "moka/assets/text_loader.h"
#include "moka/renderer/components/camera.h"

using namespace moka::ecs;

namespace moka::renderer::wrappers
{

Shader::Shader() {}

Shader::Shader(const Shader& other)
{
  *this = other;
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath, bool absolutePath)
{
  CreateShader(vertexPath, fragmentPath);
}

Shader& Shader::operator=(const Shader& other)
{
  _program = other._program;
  initialized = other.initialized;
  return *this;
}

bool Shader::CreateShader(const std::string& vertexPath, const std::string& fragmentPath, bool absolutePath)
{
  std::string vertFinalPath;
  std::string fragFinalPath;
  std::string vertSrc = moka::assets::readFile(vertexPath, absolutePath, &vertFinalPath);
  std::string fragSrc = moka::assets::readFile(fragmentPath, absolutePath, &fragFinalPath);

  bool success = CreateShaderFromString(vertSrc, fragSrc);
  if (!success)
  {
    std::cout << "Shader loading failed!" << std::endl;
    std::cout << "Vertex Shader at path `" << vertFinalPath << "`: \n```\n" << vertSrc << "```\n" << std::endl;
    std::cout << "Fragment Shader at path `" << fragFinalPath << "`: \n```\n" << fragSrc << "```\n" << std::endl;
  }

  if (success)
  {
    initialized = true;

    Use();

    glm::mat4 model = ECS::Get().GetComponentP<components::Camera>(misc::Singleton<MainLoop>::Ref().mainCamera)->DefaultModel();
    glm::mat4 proj = ECS::Get().GetComponentP<components::Camera>(misc::Singleton<MainLoop>::Ref().mainCamera)->Projection();
    Uniform("model", model);
    Uniform("projection", proj);
  }

  return success;
}

bool Shader::CreateShaderFromString(const std::string& vertexSource, const std::string& fragmentSource)
{
  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);

  const char* vs = vertexSource.c_str();
  glShaderSource(vertexShader, 1, &vs, NULL);
  glCompileShader(vertexShader);

  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

  if(!success)
  {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
    infoLog << std::endl;
    return false;
  }

  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

  const char* fs = fragmentSource.c_str();
  glShaderSource(fragmentShader, 1, &fs, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

  if(!success)
  {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" <<
    infoLog << std::endl;
    return false;
  }

  _program = glCreateProgram();
  
  glAttachShader(_program, vertexShader);
  glAttachShader(_program, fragmentShader);
  glLinkProgram(_program);

  glGetProgramiv(_program, GL_LINK_STATUS, &success);
  if(!success) {
    glGetProgramInfoLog(_program, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" <<
    infoLog << std::endl;
    return false;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return true;
}

const int Shader::Id() const
{
  return _program;
}

void Shader::Use() const
{
  glUseProgram(_program);
}

void Shader::Uniform(const std::string& var, int value) const
{
  Use();
  glUniform1i(glGetUniformLocation(_program, var.c_str()), value);
}

void Shader::Uniform(const std::string& var, bool value) const
{
  Use();
  glUniform1i(glGetUniformLocation(_program, var.c_str()), (int)value);
}

void Shader::Uniform(const std::string& var, float value) const
{
  Use();
  glUniform1f(glGetUniformLocation(_program, var.c_str()), value);
}

void Shader::Uniform(const std::string& var, const glm::mat4& value) const
{
  Use();
  int loc = glGetUniformLocation(_program, var.c_str());
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::Uniform(const std::string& var, const glm::vec4& value) const
{
  Use();
  glUniform4fv(glGetUniformLocation(_program, var.c_str()), 1, glm::value_ptr(value));
}

void Shader::Uniform(const std::string& var, const glm::vec3& value) const
{
  Use();
  glUniform3fv(glGetUniformLocation(_program, var.c_str()), 1, glm::value_ptr(value));
}

void Shader::Uniform(const std::string& var, const glm::vec2& value) const
{
  Use();
  glUniform2fv(glGetUniformLocation(_program, var.c_str()), 1, glm::value_ptr(value));
}

}
