#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <vector>
#include <imgui.h>
#include <filesystem>

#include "abcg.hpp"
#include "camera.hpp"
#include "ground.hpp"
#include "wall.hpp"

struct Vertex {
  glm::vec3 position{};
  glm::vec3 normal{};
  glm::vec2 texCoord{};

  bool operator==(const Vertex& other) const noexcept {
    static const auto epsilon{std::numeric_limits<float>::epsilon()};
    return glm::all(glm::epsilonEqual(position, other.position, epsilon)) &&
           glm::all(glm::epsilonEqual(normal, other.normal, epsilon));
           glm::all(glm::epsilonEqual(texCoord, other.texCoord, epsilon));
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

 private:
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint m_program{};

  int m_viewportWidth{};
  int m_viewportHeight{};
  int upright = 1;
  int m_trianglesToDraw{};
  int m_mappingMode = 3;

  Camera m_camera;
  float m_dollySpeed{0.0f};
  float m_truckSpeed{0.0f};
  float m_panSpeed{0.0f};
  float m_vertSpeed{0.0f};
  float rotatefront{0.0f};
  float rotateback{0.0f};
  float smallpos{0.0f};
  float yposfront{0.5};
  float yposback{0.0};

  Ground m_ground;
  Wall m_wall;

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  ImFont* m_font{};
  
  glm::mat4 m_modelMatrix{1.0f};
  glm::mat4 m_viewMatrix{1.0f};

  glm::vec4 m_lightDir{0.0f, 0.0f, -1.0f, 1.0f};
  glm::vec4 m_Ia{1.0f};
  glm::vec4 m_Id{1.0f};
  glm::vec4 m_Is{1.0f};
  glm::vec4 m_Ka;
  glm::vec4 m_Kd;
  glm::vec4 m_Ks;
  float m_shininess;
  GLuint m_diffuseTexture{};

  bool m_hasNormals{false};
  bool m_hasTexCoords{false};

  [[nodiscard]] int getNumTriangles() const {
    return static_cast<int>(m_indices.size()) / 3;
  }

  [[nodiscard]] glm::vec4 getKa() const { return m_Ka; }
  [[nodiscard]] glm::vec4 getKd() const { return m_Kd; }
  [[nodiscard]] glm::vec4 getKs() const { return m_Ks; }
  [[nodiscard]] float getShininess() const { return m_shininess; }

  [[nodiscard]] bool isUVMapped() const { return m_hasTexCoords; }

  void computeNormals();
  void loadModelFromFile(std::string_view path);
  void update();
  void loadDiffuseTexture(std::string_view path);
  void render(int numTriangles = -1) const;
};

#endif