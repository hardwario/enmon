cmake_minimum_required(VERSION 3.13)

project(enmon LANGUAGES C)

set(VERSION "vdev")

if(DEFINED ENV{VERSION})
    set(VERSION $ENV{VERSION})
endif()

message(STATUS "version ${VERSION}")

add_definitions(-DVERSION="${VERSION}")

add_executable(enmon)
add_subdirectory(src)

set_property(TARGET enmon PROPERTY C_STANDARD 11)
set_property(TARGET enmon PROPERTY C_STANDARD_REQUIRED ON)

message(STATUS "compiler ${CMAKE_C_COMPILER_ID}")

if(MSVC)
    target_compile_options(enmon PRIVATE /W3 /GL /Od /Oi /Gy /Zi /EHsc)
else()
    target_compile_options(enmon PRIVATE -Wall -Wextra -pedantic -O3)
endif()
