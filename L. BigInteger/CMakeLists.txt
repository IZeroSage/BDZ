cmake_minimum_required(VERSION 3.28)
project(jfa)

SET (CMAKE_CXX_COMPILER "clang++")
set(CMAKE_OSX_ARCHITECTURES "arm64")


set(CMAKE_CXX_STANDARD 20)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

add_executable(main_geor big_integer_test.cpp big_integer_good.cpp big_integer_good.hpp)