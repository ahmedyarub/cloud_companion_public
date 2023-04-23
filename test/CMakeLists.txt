cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(CloudCompanionTests LANGUAGES CXX)

# ---- Options ----

option(ENABLE_TEST_COVERAGE "Enable test coverage" OFF)
option(TEST_INSTALLED_VERSION "Test the version found by find_package" OFF)

# --- Import tools ----

include(../cmake/tools.cmake)

# ---- Dependencies ----

include(../cmake/CPM.cmake)

CPMAddPackage(
  NAME doctest
  GITHUB_REPOSITORY onqtam/doctest
  GIT_TAG 2.4.9
)

if(TEST_INSTALLED_VERSION)
  find_package(CloudCompanion REQUIRED)
else()
  CPMAddPackage(NAME CloudCompanion SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
endif()

CPMAddPackage(
  NAME Format.cmake
  GITHUB_REPOSITORY TheLartians/Format.cmake
  VERSION 1.7.3
  OPTIONS # enable cmake formatting
          "FORMAT_CHECK_CMAKE ON"
)

# ---- Create binary ----

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)
add_executable(CloudCompanionTests ${sources})
target_link_libraries(CloudCompanionTests doctest CloudCompanion)

set_target_properties(CloudCompanionTests PROPERTIES CXX_STANDARD 17)

# enable compiler warnings
if(NOT TEST_INSTALLED_VERSION)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(CloudCompanion PUBLIC -Wall -pedantic -Wextra -Werror)
  elseif(MSVC)
    #target_compile_options(CloudCompanion PUBLIC /W4 /WX)
    target_compile_definitions(CloudCompanionTests PUBLIC DOCTEST_CONFIG_USE_STD_HEADERS)
  endif()
endif()

# ---- Add CloudCompanionTests ----

enable_testing()

# Note: doctest and similar testing frameworks can automatically configure CMake tests For other
# testing frameworks add the tests target instead: ADD_TEST(CloudCompanionTests CloudCompanionTests)

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(CloudCompanionTests)

# ---- code coverage ----

if(ENABLE_TEST_COVERAGE)
  target_compile_options(CloudCompanion PUBLIC -O0 -g -fprofile-arcs -ftest-coverage)
  target_link_options(CloudCompanion PUBLIC -fprofile-arcs -ftest-coverage)
endif()
