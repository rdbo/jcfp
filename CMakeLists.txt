cmake_minimum_required(VERSION 3.20)

project(jcfp CXX)

option(JCFP_BUILD_TESTS "Enable JCFP test executable")

set(JCFP_INCLUDE "${PROJECT_SOURCE_DIR}/include")
file(GLOB_RECURSE JCFP_SOURCE "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_library(jcfp ${JCFP_SOURCE})
target_include_directories(jcfp PUBLIC ${JCFP_INCLUDE})

if(${JCFP_BUILD_TESTS})
  file(GLOB_RECURSE JCFP_TEST_SOURCE "${PROJECT_SOURCE_DIR}/tests/*.cpp")
  add_executable(test ${JCFP_TEST_SOURCE})
  target_include_directories(test PUBLIC ${JCFP_INCLUDE})
  target_link_libraries(test PUBLIC jcfp)
endif()
