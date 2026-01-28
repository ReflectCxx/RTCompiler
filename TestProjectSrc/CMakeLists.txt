cmake_minimum_required(VERSION 3.20)
project(CxxTestProps LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} STATIC "")

target_include_directories(${PROJECT_NAME} PRIVATE inc)

add_subdirectory(src)
