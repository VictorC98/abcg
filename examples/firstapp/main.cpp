#include "abcg.hpp"
#include "openglwindow.hpp"

int main(int argc, char **argv) {
  // Create application instance
  abcg::Application app(argc, argv);
  
  // Create OpenGL window
  auto window{std::make_unique<OpenGLWindow>()};
  window->setWindowSettings({.title = "First App"});
  
  // Run application
  app.run(std::move(window));
  
  return 0;
}
