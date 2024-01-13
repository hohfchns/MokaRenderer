#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "moka/event/signal.hpp"

namespace moka::renderer
{

class Renderer {
  friend class MainLoop;

protected:
  Renderer();
  Renderer(int windowWidth, int windowHeight);

  void ProvideDeltaTime(float dt);

  void ProcessInput();
  void Process(float deltaTime);

  void RenderStart() const;
  void RenderEnd() const;

  void ShutDown();
  void CloseWindow();

public:
  bool InitWindow();
  GLFWwindow* GetWindow();
  bool WindowShouldClose() const;

  int WWidth() const;
  int WHeight() const;

  double MouseX() const;
  double MouseY() const;

  float DeltaTime() const;

  static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
  static void MouseInputCallback(GLFWwindow* window, double xpos, double ypos);

  struct MouseInputParams { GLFWwindow* window; double xpos; double ypos; };
  static event::Signal<MouseInputParams> eventMouseInput;

  event::Signal<Renderer*> eventInputProcess;
  event::Signal<float> eventProcess;
private:
  GLFWwindow* _window;
  int _windowWidth;
  int _windowHeight;
  float _deltaTime;

  static double _mouse_xpos;
  static double _mouse_ypos;
};

}
