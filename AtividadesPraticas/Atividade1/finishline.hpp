#ifndef FINISHLINE_HPP_
#define FINISHLINE_HPP_

#include "abcg.hpp"
#include "frog.hpp"
#include "gamedata.hpp"

class OpenGLWindow;

class FinishLine {
 public:
  void initializeGL(GLuint program);
  void paintGL(const GameData &gameData);
  void terminateGL();
  void update(const Frog &frog, float deltaTime);

 private:
  friend OpenGLWindow;
  friend Frog;

  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_scaleLoc{};
  GLint m_rotationLoc{};
  GLint m_translationLoc{};
  
  GLuint m_vao{};
  GLuint m_vbo{};
  GLuint m_ebo{};

  glm::vec4 m_color{1};
  float m_rotation{};
  float m_scale{0.125f};
  bool m_hit{false};
  glm::vec2 m_translation{glm::vec2(0)};
  glm::vec2 m_velocity{glm::vec2(0)};

};

#endif