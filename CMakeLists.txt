cmake_minimum_required (VERSION 3.8)
project (multini)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} INTERFACE
  multini.hh
)
target_include_directories(${PROJECT_NAME} INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# Check if cmake is being used directly or via add_subdirectory
if(NOT MULTINI_TESTS AND CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MULTINI_TESTS ON)
endif()

if (MULTINI_TESTS)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Add source to this project's executable.
add_executable (multini_tests multini_tests.cc multini.hh)

target_link_libraries(
   multini_tests
   ${PROJECT_NAME}
   GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(multini_tests)

endif()