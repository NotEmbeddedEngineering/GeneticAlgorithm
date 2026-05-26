cmake_minimum_required(VERSION 4.2.0)
project(GeneticAlgorithm)

set(CMAKE_CXX_STANDARD 23)
add_compile_options(-Wall -Wextra -Wpedantic)
include_directories("./include")
file(GLOB_RECURSE sourceFiles ./src/*.cpp)

add_executable(${PROJECT_NAME} ${sourceFiles})
