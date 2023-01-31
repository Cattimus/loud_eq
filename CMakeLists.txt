cmake_minimum_required(VERSION 2.15)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(main)

set(SOURCES
  src/main.cpp

  src/wav.cpp
  src/wav.hpp

  src/compressor.cpp
  src/compressor.hpp)

add_compile_options(-Wall -g)

include_directories(src)
add_executable(main ${SOURCES})
