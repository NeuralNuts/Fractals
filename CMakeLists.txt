cmake_minimum_required(VERSION 3.10)
project(Fractals)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

add_executable(Fractals main.cpp)

target_link_libraries(Fractals glfw GLEW)
target_link_libraries(Fractals OpenGL::GL)