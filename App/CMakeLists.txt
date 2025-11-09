#
# App
#

set(SOURCES
        Source/Main.cpp

        Source/UI/AppLayer.cpp
        Source/UI/AppLayer.h
        Source/UI/DebugLayer.cpp
        Source/UI/DebugLayer.h

        Source/Model/Task.cpp
        Source/Model/Task.h
        Source/Model/TaskList.cpp
        Source/Model/TaskList.h
)

add_executable(App)

target_sources(App PRIVATE ${SOURCES})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

target_link_libraries(App
        PRIVATE
        Core
        SQLiteCpp
)

target_include_directories(App
        PRIVATE
        Source
        ${raygui_SOURCE_DIR}/src
)

set_target_properties(App PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/App"
)

add_custom_command(
        TARGET App POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:App>/assets
        COMMENT "Copiando assets al directorio de compilación..."
)