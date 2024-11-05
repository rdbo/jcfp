cmake_minimum_required(VERSION 3.20)

project(jcfp CXX)

set(JCFP_INCLUDE "${PROJECT_SOURCE_DIR}/include")
file(GLOB_RECURSE JCFP_SOURCE "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_library(jcfp ${JCFP_SOURCE})
target_include_directories(jcfp PUBLIC ${JCFP_INCLUDE})
