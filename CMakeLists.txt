cmake_minimum_required(VERSION 3.0)
project(crypto-tool)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_subdirectory(GmSSL EXCLUDE_FROM_ALL)

include_directories(
    GmSSL/include
    src
    src/cmdparser
)

file(GLOB src
    "src/*.c"
    "src/cmd/*.c"
    "src/xio/*.c"
    "src/cmdparser/cmdparser.c"
)

add_executable(xx ${src})
target_link_libraries(xx gmssl)

if(UNIX)
    INSTALL(TARGETS xx RUNTIME DESTINATION bin)
endif()
