find_library(LIB_HID hid)
find_library(LIB_SETUPAPI setupapi)
target_link_libraries(enmon ${LIB_HID} ${LIB_SETUPAPI})
target_sources(enmon PRIVATE hid.c)
