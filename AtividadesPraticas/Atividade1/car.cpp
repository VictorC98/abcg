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

    // Make sure the vehicle won't collide with the ship
    do {
      vehicle.m_translation = {m_randomDist(m_randomEngine),
                                m_randomDist(m_randomEngine)};
    } while (glm::length(vehicle.m_translation) < 0.5f);
  }
}

void Car::paintGL() {
  abcg::glUseProgram(m_program);

  for (const auto &vehicle : m_car) {
    abcg::glBindVertexArray(vehicle.m_vao);

    abcg::glUniform4fv(m_colorLoc, 1, &vehicle.m_color.r);
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

void Car::update(float deltaTime){
  for (auto &vehicle : m_car) {
    vehicle.m_translation += vehicle.m_velocity * deltaTime;

    // Wrap-around
    if (vehicle.m_translation.x < -1.0f) vehicle.m_translation.x += 2.0f;
    if (vehicle.m_translation.x > +1.0f) vehicle.m_translation.x -= 2.0f;
  }
}

Car::Vehicle Car::createVehicle(glm::vec2 translation,
                                              float scale) {
  Vehicle vehicle;

  auto &re{m_randomEngine};  // Shortcut

  // Randomly choose the number of sides
  std::uniform_int_distribution<int> randomSides(6, 20);
  vehicle.m_polygonSides = randomSides(re);

  // Choose a random color (actually, a grayscale)
  std::uniform_real_distribution<float> randomIntensity(0.5f, 1.0f);
  vehicle.m_color = glm::vec4(1) * randomIntensity(re);

  vehicle.m_color.a = 1.0f;
  vehicle.m_rotation = 0.0f;
  vehicle.m_scale = scale;
  vehicle.m_translation = translation;

  // Choose a random angular velocity
  vehicle.m_angularVelocity = 0.0f;

  // Choose a random direction
  glm::vec2 direction{1.0f, 0.0f};
  vehicle.m_velocity = glm::normalize(direction) / 2.0f;

  // Create geometry
  std::vector<glm::vec2> positions(0);
  positions.emplace_back(0, 0);
  const auto step{M_PI * 2 / vehicle.m_polygonSides};
  std::uniform_real_distribution<float> randomRadius(0.8f, 1.0f);
  for (const auto angle : iter::range(0.0, M_PI * 2, step)) {
    const auto radius{randomRadius(re)};
    positions.emplace_back(radius * std::cos(angle), radius * std::sin(angle));
  }
  positions.push_back(positions.at(1));

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
