#include "moka/renderer/main_loop.h"
#include <iostream>
#include <string>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "moka/assets/raw_image.h"
#include "moka/world/components/transform.h"
#include "moka/renderer/components/camera.h"
#include "moka/renderer/components/instanced_mesh.h"
#include "moka/renderer/components/model.h"
#include "moka/renderer/renderer.h"
#include "moka/renderer/wrappers/shader.h"

using namespace glm;

using namespace moka::ecs;
using namespace moka::world::components;
using namespace moka::renderer::components;
using namespace moka::renderer::wrappers;
using namespace moka::renderer::types;

namespace moka::renderer
{

static const double DELTA_TIME_MAX_SECONDS = 0.25;
static const double PHYSICS_TICK_RATE = 1.0 / 144.0;

MainLoop::MainLoop()
{
  AddComponent<Transform>();
  AddComponent<Camera>();

  mainCamera = GetEntity();

  stbi_set_flip_vertically_on_load(true);

  renderer.eventInputProcess.Connect(std::bind(&MainLoop::ProcessInput, this, std::placeholders::_1));
  Renderer::eventMouseInput.Connect(std::bind(&MainLoop::MouseInput, this, std::placeholders::_1));
  renderer.eventProcess.Connect(std::bind(&MainLoop::Process, this, std::placeholders::_1));

  renderer.InitWindow();
  glfwSetInputMode(renderer.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glEnable(GL_DEPTH_TEST);
}

int MainLoop::Run()
{
  if (renderer.GetWindow() == nullptr)
  {
    return -1;
  }

  Shader shader("resources/base.vertex.glsl", "resources/base_material.fragment.glsl");
  
  double deltaTime = 0.0;
  double lastFrame = 0.0;
  double accumulator = 0.0;

  while(!renderer.WindowShouldClose())
  {
    double currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    if (deltaTime >= DELTA_TIME_MAX_SECONDS)
    {
      deltaTime = DELTA_TIME_MAX_SECONDS;
    }
    accumulator += deltaTime;

    while (accumulator >= PHYSICS_TICK_RATE)
    {
      renderer.ProvideDeltaTime(deltaTime);
      lastFrame = currentFrame;

      renderer.ProcessInput();

      renderer.Process(PHYSICS_TICK_RATE);

      accumulator -= PHYSICS_TICK_RATE;
    }

    renderer.RenderStart();

    auto* instanced_meshes = ECS::Get().GetComponentsOfType<InstancedMesh>();
    if (instanced_meshes != nullptr)
    {
      for (auto& mesh : *instanced_meshes)
      {
        mesh.Draw();
      }
    }

    auto* models = ECS::Get().GetComponentsOfType<Model>();
    if (models != nullptr)
    {
      for (auto& elem : *models)
      {
        auto elemShader = elem.GetShader();
        if (elemShader.initialized)
        {
          elem.Draw();
        }
        else
        {
          elem.Draw(shader);
        }
      }
    }

    renderer.RenderEnd();
  }
  
  renderer.ShutDown();
  return 0;
}

Renderer* MainLoop::GetRenderer()
{
  return &renderer;
}

void MainLoop::ProcessInput(Renderer* renderer)
{
  GLFWwindow* window = renderer->GetWindow();

  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    renderer->CloseWindow();
  }

  vec3 move_vector = vec3(0, 0, 0);
  auto& camera = GetComponent<Camera>();
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
  {
    move_vector += camera.GetTransform().Front();
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
  {
    move_vector -= camera.GetTransform().Front();
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
  {
    move_vector += camera.GetTransform().Right();
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
  {
    move_vector -= camera.GetTransform().Right();
  }
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
  {
    move_vector += camera.GetTransform().Up();
  }
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
  {
    move_vector -= camera.GetTransform().Up();
  }

  float moveSpeed = 2.5f;
  camera.GetTransform().Move(move_vector * moveSpeed * renderer->DeltaTime());
  // std::cout << glm::to_string(move_vector) << "\n";
}

void MainLoop::Process(float deltaTime)
{
}

void MainLoop::MouseInput(Renderer::MouseInputParams input)
{
  auto& camera = GetComponent<Camera>();
  if (camera.GetEntity() == mainCamera)
  {
    camera.RotateFPS(lastX, lastY, input.xpos, input.ypos);
  }
}

}
