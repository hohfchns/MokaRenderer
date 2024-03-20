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
  void Move(const glm::vec3& by);

  void Rotate(float yaw, float pitch);

  void RotateFPS(float& lastX, float& lastY, float mouseXpos, float mouseYpos);

  glm::mat4 DefaultModel() const;
  glm::mat4 Projection() const;
  glm::mat4 View() const;

  moka::world::components::Transform& GetTransform();
  const moka::world::components::Transform& GetTransform() const;

  void _Init() override;
};

}
