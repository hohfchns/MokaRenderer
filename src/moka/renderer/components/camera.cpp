#include "moka/renderer/components/camera.h"
#include "moka/ecs/core/ecs.hpp"
#include "moka/renderer/main_loop.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <stdexcept>

using namespace glm;
using namespace moka::ecs;
using namespace moka::world::components;

namespace moka::renderer::components
{

void Camera::_Init()
{
}

void Camera::Rotate(float roll, float pitch)
{
  vec3 curRot = GetComponent<Transform>().Rotation();
  curRot += vec3(roll, pitch, 0.f);
  if (curRot.x > 90.f)
  {
    curRot.x = 90.f;
  }
  if (curRot.x < -90.f)
  {
    curRot.x = -90.f;
  }

  GetComponent<Transform>().SetRotation(curRot);
}

mat4 Camera::View() const
{
  return lookAt(GetComponent<Transform>().GlobalPosition(), GetComponent<Transform>().GlobalPosition() + GetComponent<Transform>().Front(), GetComponent<Transform>().Up());
}

glm::mat4 Camera::DefaultModel() const
{
  return mat4(1.f);
}

glm::mat4 Camera::Projection() const
{
  auto* renderer = misc::Singleton<MainLoop>::Ref().GetRenderer();
  float ratio = (float)renderer->WWidth() / (float)renderer->WHeight();

  if (persp == Perspective::PERSPECTIVE)
  {
    return perspective(radians(fov), ratio, near, far);
  }

  auto trans = GetTransform();
  auto scale = trans.Scale();
  return ortho(-scale.x, scale.x, -scale.y, scale.y);
}

void Camera::RotateFPS(float& lastX, float& lastY, float mouseXpos, float mouseYpos)
{
  static bool firstMouse = false;
  if (firstMouse)
  {
    lastX = mouseXpos;
    lastY = mouseYpos;
    firstMouse = true;
  }

  float xoffset = mouseXpos - lastX;
  float yoffset = lastY - mouseYpos;
  lastX = mouseXpos;
  lastY = mouseYpos;

  const float sensitivity = 0.1f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  Rotate(yoffset, xoffset);
}

Transform& Camera::GetTransform()
{
  return GetComponent<Transform>();
}

const Transform& Camera::GetTransform() const
{
  return GetComponent<Transform>();
}

void Camera::SetPerspective(Perspective persp)
{
  this->persp = persp;
}

Camera::Perspective Camera::GetPerspective()
{
  return persp;
}

void Camera::SetFOV(float fov)
{
  this->fov = fov;
}

float Camera::GetFOV()
{
  return fov;
}

void Camera::SetNear(float near)
{
  this->near = near;
}

float Camera::GetNear()
{
  return near;
}

void Camera::SetFar(float far)
{
  this->far = far;
}

float Camera::GetFar()
{
  return far;
}

}
