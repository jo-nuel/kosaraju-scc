cmake_minimum_required(VERSION 3.20)

project(kosaraju_scc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(scc src/main.cpp)
add_executable(scc_tests tests/scc_tests.cpp)

if(MSVC)
  target_compile_options(scc PRIVATE /W4)
  target_compile_options(scc_tests PRIVATE /W4)
else()
  target_compile_options(scc PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(scc_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_test(NAME project_setup COMMAND scc_tests)

