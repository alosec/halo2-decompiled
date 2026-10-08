cmake_minimum_required(VERSION 3.24)
project(halo2_native_port LANGUAGES C CXX)

include(CTest)

option(H2_BUILD_NATIVE_PORT "Build the experimental native host runtime" ON)

if(H2_BUILD_NATIVE_PORT)
    add_subdirectory(port)
endif()
