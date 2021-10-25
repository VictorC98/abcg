#include "car.hpp"

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Car::initializeGL(GLuint program, int quantity) {
  terminateGL();

  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  m_program = program;
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  // Create vehicles
  m_car.clear();
  m_car.resize(quantity);

  for (auto &vehicle : m_car) {
    vehicle = createVehicle();

    // Make sure the vehicle won't collide with the frog
    do {
      vehicle.m_translation = {m_randomDist(m_randomEngine),
                                m_randomDist(m_randomEngine)};
    } while (glm::length(vehicle.m_translation) < 0.5);
  }
}

void Car::paintGL() {
  abcg::glUseProgram(m_program);

  for (const auto &vehicle : m_car) {
    abcg::glBindVertexArray(vehicle.m_vao);

    abcg::glUniform4fv(m_colorLoc, 1, &vehicle.m_color.a);
    abcg::glUniform1f(m_scaleLoc, vehicle.m_scale);
    abcg::glUniform1f(m_rotationLoc, vehicle.m_rotation);

    for (auto i : {-2, 0, 2}) {
      for (auto j : {-2, 0, 2}) {
        abcg::glUniform2f(m_translationLoc, vehicle.m_translation.x + j,
                          vehicle.m_translation.y + i);

        abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, vehicle.m_polygonSides + 2);
      }
    }

    abcg::glBindVertexArray(0);
  }

  abcg::glUseProgram(0);
}

void Car::terminateGL() {
  for (auto vehicle : m_car) {
    abcg::glDeleteBuffers(1, &vehicle.m_vbo);
    abcg::glDeleteVertexArrays(1, &vehicle.m_vao);
  }
}

void Car::update(const Frog &frog, float deltaTime) {
  for (auto &vehicle : m_car) {
    vehicle.m_translation -= frog.m_velocity * deltaTime;
    vehicle.m_translation += vehicle.m_velocity * deltaTime;
    // Wrap-around
    if (vehicle.m_translation.x < -1.0f) vehicle.m_translation.x += 2.0f;
    if (vehicle.m_translation.x > +1.0f) vehicle.m_translation.x -= 2.0f;
  }
}

Car::Vehicle Car::createVehicle(glm::vec2 translation, float scale) {
  Vehicle vehicle;

  vehicle.m_polygonSides = 4;

  vehicle.m_color.a = 1.0f;
  vehicle.m_rotation = 0.0f;
  vehicle.m_scale = scale;
  vehicle.m_translation = translation;
  vehicle.m_angularVelocity = 0.0f;

  // Choose a random direction
  float d = rand() % 2;
  if (d == 0){
    d = -1.0;
  }
  float speedmod = rand() % 4 + 1; //velocidade aleatória para cada carro

  glm::vec2 direction{d, 0.0f};
  vehicle.m_velocity = glm::normalize(direction) * speedmod / 7.0f;

  // Create geometry
  std::array<glm::vec2, 4> positions{
      glm::vec2{-08.0f, +08.0f}, glm::vec2{-08.0f, -08.0f},
      glm::vec2{+12.0f, -08.0f}, glm::vec2{+12.0f, +08.0f},
      };

  // Normalize
  for (auto &position : positions) {
    position /= glm::vec2{18.0f, 18.0f};
  }


  // Generate VBO
  abcg::glGenBuffers(1, &vehicle.m_vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, vehicle.m_vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &vehicle.m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(vehicle.m_vao);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, vehicle.m_vbo);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);

  return vehicle;
}
