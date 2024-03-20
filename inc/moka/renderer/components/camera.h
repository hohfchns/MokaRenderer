#pragma once
#include "moka/ecs/core/component.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include "moka/world/components/transform.h"
#include "moka/renderer/renderer.h"

namespace moka::renderer::components
{

class Camera : public moka::ecs::Component
{
public:
  enum class Perspective {
    PERSPECTIVE = 0,
    ORTHOGRAPHIC,
  };

  void Move(const glm::vec3& by);

  void Rotate(float yaw, float pitch);

  void RotateFPS(float& lastX, float& lastY, float mouseXpos, float mouseYpos);

  glm::mat4 DefaultModel() const;
  glm::mat4 Projection() const;
  glm::mat4 View() const;

  moka::world::components::Transform& GetTransform();
  const moka::world::components::Transform& GetTransform() const;

  void SetPerspective(Perspective persp);
  Perspective GetPerspective();
  void SetFOV(float fov);
  float GetFOV();
  void SetNear(float near);
  float GetNear();
  void SetFar(float far);
  float GetFar();

  void _Init() override;

private:
  Perspective persp = Perspective::PERSPECTIVE;
  // Field of View in degrees
  float fov = 45.f;
  float near = 0.1f;
  float far = 2000.f;
};

}
