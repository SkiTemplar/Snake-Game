cmake_minimum_required(VERSION 3.21)
project(Snake CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(Snake src/main.cpp
        src/App.cpp
        src/App.h)

target_link_libraries(Snake PRIVATE glfw glm::glm glad::glad opengl32)

