cmake_minimum_required(VERSION 3.0)
project(crypto-tool)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_definitions(-DENABLE_BROKEN_CRYPTO)
option(ENABLE_BROKEN_CRYPTO "Enable broken crypto" ON)
option(ENABLE_RDRND "Enable Intel RDRND instructions" ON)
add_subdirectory(GmSSL EXCLUDE_FROM_ALL)

include_directories(
    GmSSL/include
    cmdparser
    src
)

file(GLOB_RECURSE src
    "src/*.c"
    "cmdparser/cmdparser.c"
)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_executable(xx ${src})
target_link_libraries(xx gmssl)

if(UNIX)
    INSTALL(TARGETS xx RUNTIME DESTINATION bin)
endif()

execute_process(
    COMMAND git describe --tags --long --dirty=-modified --always
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE GIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_definitions(-DGIT_TAG="${GIT_TAG}")
message(STATUS "GIT_TAG: ${GIT_TAG}")

message(STATUS "CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")