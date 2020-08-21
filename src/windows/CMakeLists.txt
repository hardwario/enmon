set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

find_package(WinHID REQUIRED)

target_link_libraries(enmon ${WINHID_LIBRARIES})
target_include_directories(enmon PRIVATE ..)
target_sources(enmon PRIVATE hid.c)
