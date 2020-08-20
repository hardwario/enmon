target_sources(enmon
    PRIVATE
        argtable3.c
        bridge.c
        cli.c
        ft260.c
        main.c
        mpl3115a2.c
        opt3001.c
        sht20.c
        util.c
)
if(WIN32)
    add_subdirectory(windows)
elseif(APPLE)
    add_subdirectory(macos)
    target_link_libraries(enmon m)
else()
    add_subdirectory(linux)
    target_link_libraries(enmon m)
endif()
