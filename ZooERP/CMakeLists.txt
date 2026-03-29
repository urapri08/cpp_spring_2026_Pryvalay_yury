cmake_minimum_required(VERSION 3.10)
project(ZooERP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(MSVC)
    add_compile_options(/utf-8)
endif()

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_executable(ZooERP ${SOURCES})