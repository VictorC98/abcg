#include "frog.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Frog::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  m_rotation = 0.0f;
  m_translation = glm::vec2(0);
  m_velocity = glm::vec2(0);

  std::array<glm::vec2, 22> positions{
      // Frog Torso
      glm::vec2{-04.0f, +12.0f}, glm::vec2{-12.0f, +04.0f},
      glm::vec2{-12.0f, -12.0f}, glm::vec2{+12.0f, -12.0f},
      glm::vec2{+12.0f, +04.0f}, glm::vec2{+04.0f, +12.0f},

      // Left front leg
      glm::vec2{-13.0f, +12.0f}, glm::vec2{-13.0f, +02.0f},
      glm::vec2{-09.5f, +02.0f}, glm::vec2{-09.5f, +12.0f},

      // Right front leg
      glm::vec2{+09.5f, +12.0f}, glm::vec2{+09.5f, +02.0f},
      glm::vec2{+13.0f, +02.0f}, glm::vec2{+13.0f, +12.0f},
      
      // Left back Leg
      glm::vec2{-10.0f, -12.0f}, glm::vec2{-12.0f, -22.0f},
      glm::vec2{-07.0f, -22.0f}, glm::vec2{-04.0f, -12.0f},

      // Right back leg
      glm::vec2{+04.0f, -12.0f}, glm::vec2{+07.0f, -22.0f},
      glm::vec2{+12.0f, -22.0f}, glm::vec2{+10.0f, -12.0f},

      };

  // Normalize
  for (auto &position : positions) {
    position /= glm::vec2{18.0f, 18.0f};
  }

  const std::array indices{0, 1, 17,
                           1, 2, 17,
                           0, 17, 18,
                           0, 5, 18,
                           5, 18, 4,
                           3, 4, 18,
                           // Front Legs
                           6, 7, 9,
                           7, 8, 9,
                           10, 11, 12,
                           10, 12, 13,
                           // Back Legs
                           14, 15, 16,
                           14, 16, 17,
                           18, 19, 21,
                           19, 20, 21};

  // Generate VBO
  abcg::glGenBuffers(1, &m_vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &m_ebo);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_vao);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Frog::paintGL(const GameData &gameData) {
  if (gameData.m_state != State::Playing) return;

  abcg::glUseProgram(m_program);

  abcg::glBindVertexArray(m_vao);

  abcg::glUniform1f(m_scaleLoc, m_scale);
  abcg::glUniform1f(m_rotationLoc, m_rotation);
  abcg::glUniform2fv(m_translationLoc, 1, &m_translation.x);

  abcg::glUniform4fv(m_colorLoc, 1, &m_color.r);
  abcg::glDrawElements(GL_TRIANGLES, 14 * 3, GL_UNSIGNED_INT, nullptr);

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Frog::terminateGL() {
  abcg::glDeleteBuffers(1, &m_vbo);
  abcg::glDeleteBuffers(1, &m_ebo);
  abcg::glDeleteVertexArrays(1, &m_vao);
}

void Frog::update(const GameData &gameData, float deltaTime) {
  if (gameData.m_input[static_cast<size_t>(Input::Left)])
    m_rotation = glm::wrapAngle(m_rotation + 4.0f * deltaTime);
  if (gameData.m_input[static_cast<size_t>(Input::Right)])
    m_rotation = glm::wrapAngle(m_rotation - 4.0f * deltaTime);
  // Apply thrust
  if (gameData.m_input[static_cast<size_t>(Input::Up)] &&
      gameData.m_state == State::Playing) {
    // Thrust in the forward vector
    glm::vec2 forward = glm::rotate(glm::vec2{0.0f, 1.0f}, m_rotation);
    m_velocity += forward * deltaTime;
  }
}