cmake_minimum_required(VERSION 3.9)

project(Automatique
  VERSION 0.0.0.2
  DESCRIPTION "Automatic robotic world"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-Wshadow)
add_compile_options(-Werror)
add_compile_options(-pedantic)
add_compile_options(-pedantic-errors)
add_compile_options(-Wshorten-64-to-32)
add_compile_options(-Wfloat-equal)
add_compile_options(-fstrict-aliasing)

set(IMPLEMENTATION_SOURCES
  main.cpp
  version.cpp
)
set(IMPLEMENTATION_HEADERS
  command.h
  cxdate.hpp
  cxhash.hpp
  future_combine.hpp
  future_utils.hpp
  position.hpp
  pretty_print.hpp
  variant_utils.hpp
  version.h
  world.h
)

add_executable(${PROJECT_NAME}
  ${IMPLEMENTATION_SOURCES}
  ${IMPLEMENTATION_HEADERS}
)
