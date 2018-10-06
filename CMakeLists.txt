cmake_minimum_required(VERSION 3.9)

project(Automatique
  VERSION 0.0.0.4
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
  pretty_print.cpp
  traits.cpp
  version.cpp
  world.cpp
)
set(IMPLEMENTATION_HEADERS
  control_block.hpp
  control_module.hpp
  cxdate.hpp
  cxhash.hpp
  future_combine.hpp
  future_utils.hpp
  operation.h
  operation_queue.hpp
  position.hpp
  pretty_print.hpp
  traits.h
  unit.h
  variant_utils.hpp
  version.h
  world.h
)

add_library(${PROJECT_NAME} STATIC
  ${IMPLEMENTATION_SOURCES}
  ${IMPLEMENTATION_HEADERS}
)
target_include_directories(${PROJECT_NAME} PUBLIC .)

add_executable(${PROJECT_NAME}.app EXCLUDE_FROM_ALL main.cpp)
target_link_libraries(${PROJECT_NAME}.app PRIVATE ${PROJECT_NAME})

# set(CATCH_BUILD_TESTING OFF CACHE "No self test")
# set(CATCH_INSTALL_DOCS OFF CACHE "No docs")
# set(CATCH_INSTALL_HELPERS OFF "No helpers")

add_subdirectory(Catch2)
add_subdirectory(tests)


