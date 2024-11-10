cmake_minimum_required(VERSION 3.20)

project(jcfp CXX)

set (CMAKE_CXX_STANDARD 23)

option(JCFP_BUILD_TESTS "Enable JCFP test executable")

set(JCFP_INCLUDE "${PROJECT_SOURCE_DIR}/include")
file(GLOB_RECURSE JCFP_SOURCE "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_library(jcfp ${JCFP_SOURCE})
target_include_directories(jcfp PUBLIC ${JCFP_INCLUDE})

if(${JCFP_BUILD_TESTS})
  find_package(Java COMPONENTS Development)

  file(GLOB_RECURSE JCFP_TEST_SOURCE "${PROJECT_SOURCE_DIR}/tests/*.cpp")
  add_executable(test ${JCFP_TEST_SOURCE})
  target_include_directories(test PUBLIC ${JCFP_INCLUDE})
  target_link_libraries(test PUBLIC jcfp)

  # Build Dummy Java class
  configure_file("${PROJECT_SOURCE_DIR}/tests/Dummy.java" "${PROJECT_BINARY_DIR}/Dummy.java" COPYONLY)
  execute_process(
    COMMAND ${Java_JAVAC_EXECUTABLE} "Dummy.java"
    WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
  )
endif()
