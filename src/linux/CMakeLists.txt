# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)
if(CPACK_GENERATOR STREQUAL DEB)
    set(CPACK_COMPONENTS_ALL main dbg)
    set(CPACK_COMPONENT_DBG_DEPENDS main)
endif()

find_package(PkgConfig)
pkg_check_modules(LIBUDEV libudev)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBUDEV DEFAULT_MSG LIBUDEV_FOUND)

target_include_directories(enmon PRIVATE ..)
target_sources(enmon PRIVATE hid.c)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ludev")
target_link_libraries(${PROJECT_NAME} ${LIBUDEV_LIBRARIES})
