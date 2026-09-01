cmake_minimum_required(VERSION 3.20)

project(kosaraju_scc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(scc_core src/graph.cpp)
target_include_directories(scc_core PUBLIC include)

add_executable(scc src/main.cpp)
add_executable(scc_tests tests/scc_tests.cpp)
target_link_libraries(scc PRIVATE scc_core)
target_link_libraries(scc_tests PRIVATE scc_core)

if(MSVC)
  target_compile_options(scc_core PRIVATE /W4)
  target_compile_options(scc PRIVATE /W4)
  target_compile_options(scc_tests PRIVATE /W4)
else()
  target_compile_options(scc_core PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(scc PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(scc_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_test(NAME graph_representation COMMAND scc_tests)
