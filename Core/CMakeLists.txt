#
# Core
#

set(SOURCES
        Source/Core/Application.cpp
        Source/Core/Application.h
        Source/Core/Window.cpp
        Source/Core/Window.h
        Source/Core/Layer.h
        Source/Core/Event.h
    )

add_library(Core STATIC)

target_sources(Core PRIVATE ${SOURCES})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

target_link_libraries(Core
        PRIVATE
        raylib
)

target_include_directories(Core
        PUBLIC
        "Source"
)