cmake_minimum_required(VERSION 3.9)

project(Automatique
  VERSION 0.6.0.0
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
  src/chronicles.cpp
  src/modules/discovery.cpp
  src/modules/mining.cpp
  src/modules/warehouse.cpp
  src/traits/discover.cpp
  src/traits/mine.cpp
  src/traits/move.cpp
  src/utils/pretty_print.cpp
  src/version.cpp
  src/world.cpp
)
set(IMPLEMENTATION_HEADERS
  src/chronicles.h
  src/utils/cxdate.hpp
  src/utils/cxhash.hpp
  src/utils/future_combine.hpp
  src/utils/future.hpp
  src/modules/control.hpp
  src/modules/discovery.h
  src/modules/mining.h
  src/modules/warehouse.h
  src/utils/pretty_print.hpp
  src/primitives/control_block.hpp
  src/primitives/operation.h
  src/primitives/operation_queue.hpp
  src/traits/common.hpp
  src/traits/discover.hpp
  src/traits/mine.hpp
  src/traits/move.hpp
  src/types/position.hpp
  src/types/resourcetype.h
  src/types/tilecontent.h
  src/units/basic.h
  src/units/carrier.h
  src/utils/variant.hpp
  src/version.h
  src/world.h
)

add_library(${PROJECT_NAME} STATIC
  ${IMPLEMENTATION_SOURCES}
  ${IMPLEMENTATION_HEADERS}
)
target_include_directories(${PROJECT_NAME} PUBLIC src/)

add_executable(${PROJECT_NAME}.app src/main.cpp)
target_link_libraries(${PROJECT_NAME}.app PRIVATE ${PROJECT_NAME})

# set(CATCH_BUILD_TESTING OFF CACHE "No self test")
# set(CATCH_INSTALL_DOCS OFF CACHE "No docs")
# set(CATCH_INSTALL_HELPERS OFF "No helpers")

add_subdirectory(Catch2)
add_subdirectory(tests)


