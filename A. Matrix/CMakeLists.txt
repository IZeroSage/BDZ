cmake_minimum_required(VERSION 3.28)
project(Matrix)

SET (CMAKE_CXX_COMPILER "clang++")
set(CMAKE_OSX_ARCHITECTURES "arm64")


set(CMAKE_CXX_STANDARD 20)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

add_executable(main_run matrix_test.cpp rational.hpp rational.cpp matrix.hpp)