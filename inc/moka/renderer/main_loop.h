#pragma once
#include "moka/event/signal.hpp"
#include "moka/renderer/renderer.h"
#include "moka/ecs/core/system.hpp"

namespace moka::renderer
{

class MainLoop : public moka::ecs::System
{
  friend class misc::Singleton<MainLoop>;

public:
  int Run();
  moka::ecs::Entity mainCamera;

  Renderer* GetRenderer();

  event::Signal<moka::ecs::Entity> eventRenderEntityPre;
  event::Signal<moka::ecs::Entity> eventRenderEntityPost;

protected:
  MainLoop();

private:

  Renderer renderer;

  void MouseInput(Renderer::MouseInputParams input);
  void ProcessInput(Renderer* renderer);
  void Process(float deltaTime);

  float lastX = 800.f, lastY = 600.f;
};

}
