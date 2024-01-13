#include "moka/renderer/renderer.h"
#include <GLFW/glfw3.h>

namespace moka::renderer
{

moka::event::Signal<Renderer::MouseInputParams> Renderer::eventMouseInput;
double Renderer::_mouse_xpos = 0;
double Renderer::_mouse_ypos = 0;

Renderer::Renderer() : _windowWidth(800), _windowHeight(600)
{}

Renderer::Renderer(int windowWidth, int windowHeight) : _windowWidth(windowWidth), _windowHeight(windowHeight)
  
{}

bool Renderer::InitWindow()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  int width = 800;
  int height = 600;

  GLFWwindow* window = glfwCreateWindow(width, height, "ECS", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    _window = nullptr;
    return false;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    _window = nullptr;
    return false;
  }

  glViewport(0, 0, _windowWidth, _windowHeight);
  glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
  glfwSetCursorPosCallback(window, MouseInputCallback);

  _window = window;
  return true;
}

bool Renderer::WindowShouldClose() const
{
  return glfwWindowShouldClose(_window);
}

GLFWwindow* Renderer::GetWindow()
{
  return _window;
}

void Renderer::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void Renderer::MouseInputCallback(GLFWwindow* window, double xpos, double ypos)
{
  eventMouseInput.Emit({window, xpos, ypos});
}

void Renderer::RenderStart() const
{
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::RenderEnd() const
{
  glfwSwapBuffers(_window);
  glfwPollEvents();
}

void Renderer::ProcessInput()
{
  eventInputProcess.Emit(this);
}

void Renderer::Process(float deltaTime)
{
  eventProcess.Emit(deltaTime);
}

void Renderer::CloseWindow()
{
  glfwSetWindowShouldClose(_window, 1);
}

void Renderer::ShutDown()
{
  glfwTerminate();
}

int Renderer::WWidth() const
{
  return _windowWidth;
}

int Renderer::WHeight() const
{
  return _windowHeight;
}

double Renderer::MouseX() const
{
  return _mouse_xpos;
}

double Renderer::MouseY() const
{
  return _mouse_ypos;
}

float Renderer::DeltaTime() const
{
  return _deltaTime;
}

void Renderer::ProvideDeltaTime(float dt)
{
  _deltaTime = dt;
}
}
