cmake_minimum_required(VERSION 3.20)
project(option_pricer_hw VERSION 0.1.0 LANGUAGES CXX)
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()
if(POLICY CMP0168)
    cmake_policy(SET CMP0168 NEW)
endif()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Keep project and test libraries inside the executables by default.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries instead of static libraries")
add_library(pricing src/pricing.cpp)
target_include_directories(pricing PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_compile_options(pricing PRIVATE -Wall -Wextra -Wpedantic)
add_executable(pricing_demo app/main.cpp)
target_link_libraries(pricing_demo PRIVATE pricing)

include(FetchContent)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
FetchContent_Declare(googletest
    URL https://codeload.github.com/google/googletest/tar.gz/refs/tags/v1.15.2
    URL_HASH SHA256=7b42b4d6ed48810c5362c265a17faebe90dc2373c885e5216439d37927f02926
    TLS_VERIFY ON TIMEOUT 120 INACTIVITY_TIMEOUT 30)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(benchmark
    URL https://codeload.github.com/google/benchmark/tar.gz/refs/tags/v1.9.1
    URL_HASH SHA256=32131c08ee31eeff2c8968d7e874f3cb648034377dfc32a4c377fa8796d84981
    TLS_VERIFY ON TIMEOUT 120 INACTIVITY_TIMEOUT 30)
FetchContent_MakeAvailable(googletest benchmark)

enable_testing()
add_executable(pricing_tests tests/pricing_test.cpp)
target_link_libraries(pricing_tests PRIVATE pricing GTest::gtest_main)
add_test(NAME pricing_tests COMMAND pricing_tests)
add_test(NAME pricing_demo COMMAND pricing_demo)
set_tests_properties(pricing_tests pricing_demo PROPERTIES TIMEOUT 30)

add_executable(pricing_bench benchmarks/pricing_bench.cpp)
target_link_libraries(pricing_bench PRIVATE pricing benchmark::benchmark)

find_package(Doxygen REQUIRED)
configure_file(Doxyfile.in Doxyfile @ONLY)
add_custom_target(docs COMMAND ${DOXYGEN_EXECUTABLE} "${PROJECT_BINARY_DIR}/Doxyfile"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}" COMMENT "Generate homework documentation"
    VERBATIM)
