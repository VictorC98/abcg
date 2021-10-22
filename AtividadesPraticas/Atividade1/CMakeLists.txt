project(Frogger)

add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp frog.cpp car.cpp flag.cpp)

enable_abcg(${PROJECT_NAME})