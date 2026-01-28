cmake_minimum_required(VERSION 3.21)
project(clang-mirror LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Executable name used by inc/src CMakeLists
set(CXX_EXE_NAME clang-mirror)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

# LLVM is under <root>/build/clang-21.1.0
set(LLVM_ROOT "${CMAKE_SOURCE_DIR}/build/clang-21.1.0" CACHE PATH "Path to LLVM root")

set(LLVM_DIR  "${LLVM_ROOT}/lib/cmake/llvm")
set(Clang_DIR "${LLVM_ROOT}/lib/cmake/clang")

if (NOT EXISTS "${LLVM_DIR}/LLVMConfig.cmake")
    message(FATAL_ERROR
        "LLVM not found at ${LLVM_ROOT}\n"
        "Expected: ${LLVM_DIR}/LLVMConfig.cmake\n"
        "Please extract clang+llvm into build/clang-21.1.0 or set LLVM_ROOT.")
endif()

find_package(LLVM CONFIG REQUIRED)
find_package(Clang CONFIG REQUIRED)

add_executable(${CXX_EXE_NAME})

# Pull in headers and sources
add_subdirectory(inc)
add_subdirectory(src)

target_include_directories(${CXX_EXE_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${LLVM_INCLUDE_DIRS}
    ${CLANG_INCLUDE_DIRS}
)

llvm_map_components_to_libnames(LLVM_LIBS support)

target_link_libraries(${CXX_EXE_NAME}
    PRIVATE
    clangTooling
    clangBasic
    clangAST
    clangFrontend
    clangSerialization
    clangDriver
    clangParse
    clangSema
    clangAnalysis
    clangEdit
    clangLex
    clangRewrite
    clangRewriteFrontend
	clangTidy
    ${LLVM_LIBS}
)

if (MSVC)
    target_compile_definitions(${CXX_EXE_NAME} PRIVATE
        _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
    )

    # Treat LLVM/Clang headers as external to suppress their warnings
    target_compile_options(${CXX_EXE_NAME} PRIVATE
        /external:I"${LLVM_INCLUDE_DIRS}"
        /external:I"${CLANG_INCLUDE_DIRS}"
        /external:W0
    )
endif()
