#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <vector>
#include <imgui.h>

#include "abcg.hpp"
#include "camera.hpp"
#include "ground.hpp"
#include "wall.hpp"

struct Vertex {
  glm::vec3 position;

  bool operator==(const Vertex& other) const {
    return position == other.position;
  }
};

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& ev) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;
  void fire();

 private:
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint m_program{};

  int m_viewportWidth{};
  int m_viewportHeight{};
  int upright = 1;

  Camera m_camera;
  float m_dollySpeed{0.0f};
  float m_truckSpeed{0.0f};
  float m_panSpeed{0.0f};
  float m_vertSpeed{0.0f};
  float rotateblue{0.0f};
  float rotatered{0.0f};
  float yellowpos{0.0f};

  Ground m_ground;
  Wall m_wall;

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  ImFont* m_font{};

  void loadModelFromFile(std::string_view path);
  void update();
};

#endif