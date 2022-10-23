cmake_minimum_required(VERSION 3.0)
project(crypto-tool)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
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
