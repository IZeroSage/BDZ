cmake_minimum_required(VERSION 3.31)
project(Vector+)
set(CMAKE_CXX_STANDARD 20)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)
add_executable(final_bro vector_public_test.cpp vector.hpp)