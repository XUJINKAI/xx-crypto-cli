cmake_minimum_required(VERSION 3.0)
project(crypto-tool)

message(STATUS "CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_definitions(-DENABLE_BROKEN_CRYPTO)
option(ENABLE_BROKEN_CRYPTO "Enable broken crypto" ON)
option(ENABLE_RDRND "Enable Intel RDRND instructions" ON)
add_subdirectory(GmSSL EXCLUDE_FROM_ALL)

execute_process(
    COMMAND git describe --tags --long --dirty=-modified --always
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE GIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_definitions(-DGIT_TAG="${GIT_TAG}")
message(STATUS "GIT_TAG: ${GIT_TAG}")


include_directories(
    GmSSL/include
    cmdparser
    src
)

file(GLOB_RECURSE src
    "src/*.c"
    "cmdparser/cmdparser.c"
)
list(REMOVE_ITEM src ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
if(UNIX)
  add_compile_options(
    -pedantic
    -Wall
    # -Wextra
    -Wno-unused-parameter
    -Wno-unused-label
    -Wno-sign-compare
    -Wno-pointer-sign
    -Wno-unused-variable
    # -Wno-unused-but-set-variable
    # -Wno-unused-function
    # -Wno-missing-field-initializers
    # -Wno-newline-eof
    # -Wno-sometimes-uninitialized
    )
endif()

add_library(xxlib ${src})

add_executable(xx src/main.c)
target_link_libraries(xx xxlib gmssl)
if(UNIX)
    target_link_libraries(xx m)
endif()


if(UNIX)
    INSTALL(TARGETS xx RUNTIME DESTINATION bin)
endif()


file(GLOB_RECURSE test_src
    "test/*.c"
)
add_executable(xxtest EXCLUDE_FROM_ALL ${test_src})
target_link_libraries(xxtest xxlib gmssl)
if(UNIX)
    target_link_libraries(xxtest m)
endif()

add_custom_target(test COMMAND xxtest)
add_custom_target(test-valgrind
    COMMAND valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes
                    --trace-children=yes --error-exitcode=1
                    # --verbose --log-file=valgrind.log
                    $<TARGET_FILE:xxtest>
)
add_custom_target(clang-format COMMAND clang-format -i ${src} ${test_src})
