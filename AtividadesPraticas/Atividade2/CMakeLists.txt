project(ShootingRange)

add_executable(${PROJECT_NAME} camera.cpp main.cpp openglwindow.cpp ground.cpp wall.cpp)

enable_abcg(${PROJECT_NAME})
