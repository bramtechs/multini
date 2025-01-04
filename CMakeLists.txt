cmake_minimum_required (VERSION 3.10)
project (multini)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(${PROJECT_NAME} INTERFACE
  include/multini.hh
)

target_include_directories(${PROJECT_NAME} INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
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
  add_executable (multini_tests multini_tests.cc)

  target_link_libraries(
    multini_tests
    multini
    GTest::gtest_main
  )

  include(GoogleTest)
  gtest_discover_tests(multini_tests)

  add_executable (multini_demo
    demo.cc
  )

  target_link_libraries(multini_demo PRIVATE
    multini
  )

  if (WIN32 AND MSVC)
    # change default working directory in Visual Studio to repository root.
    set_property(TARGET multini_demo PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
    set_property(TARGET multini_tests PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

    # change default starting project in Visual Studio
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT multini_demo)

    message("Configured Visual Studio defaults")
  endif()

endif()
