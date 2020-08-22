cmake_minimum_required(VERSION 3.13)

project(enmon LANGUAGES C)

if(ENV{VERSION})
    add_definitions(-DVERSION="$ENV{VERSION}")
endif()

add_executable(enmon)
add_subdirectory(src)

set_property(TARGET enmon PROPERTY C_STANDARD 11)
set_property(TARGET enmon PROPERTY C_STANDARD_REQUIRED ON)

message(STATUS "compiler ${CMAKE_C_COMPILER_ID}")

if(MSVC)
    target_compile_options(enmon PRIVATE /W3 /GL /Od /Oi /Gy /Zi /EHsc)
else()
    target_compile_options(enmon PRIVATE -Wall -Wextra -pedantic)
endif()
