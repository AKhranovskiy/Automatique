cmake_minimum_required(VERSION 3.9)

project(Automatique.tests LANGUAGES CXX)

add_compile_options(-fprofile-instr-generate -fcoverage-mapping)

add_executable(${PROJECT_NAME}
  main.cpp
  unit_operation_queue.cpp
  unit_position.cpp
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    --coverage
    Catch2
    Automatique
)
