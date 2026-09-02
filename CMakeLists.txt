cmake_minimum_required(VERSION 3.20)

project(kosaraju_scc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(scc_core
  src/graph.cpp
  src/kosaraju.cpp
  src/tarjan.cpp
)
target_include_directories(scc_core PUBLIC include)

add_library(benchmark_support benchmarks/generators.cpp)
target_include_directories(benchmark_support PUBLIC include)
target_link_libraries(benchmark_support PUBLIC scc_core)

add_executable(scc src/main.cpp)
add_executable(scc_tests tests/scc_tests.cpp)
target_link_libraries(scc PRIVATE scc_core)
target_link_libraries(scc_tests PRIVATE scc_core benchmark_support)

if(MSVC)
  target_compile_options(scc_core PRIVATE /W4)
  target_compile_options(benchmark_support PRIVATE /W4)
  target_compile_options(scc PRIVATE /W4)
  target_compile_options(scc_tests PRIVATE /W4)
else()
  target_compile_options(scc_core PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(benchmark_support PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(scc PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(scc_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_test(NAME scc_tests COMMAND scc_tests)
