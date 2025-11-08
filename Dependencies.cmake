#
# Dependencies
#
include(FetchContent)

FetchContent_Declare(
        raylib
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.zip
)

FetchContent_GetProperties(raylib)
if(NOT raylib_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)
endif()

set_target_properties(raylib PROPERTIES FOLDER "Dependencies")

FetchContent_Declare(
        raygui
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/raysan5/raygui/archive/refs/tags/4.0.zip
)

FetchContent_Populate(raygui)

FetchContent_Declare(
        SQLiteCpp
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/SRombauts/SQLiteCpp/archive/refs/tags/3.3.1.zip
)

set(SQLITECPP_INTERNAL_SQLITE ON CACHE BOOL "Usar el FetchContent interno de SQLiteCpp para sqlite3")

FetchContent_MakeAvailable(SQLiteCpp)

set_target_properties(SQLiteCpp PROPERTIES FOLDER "Dependencies")
set_target_properties(sqlite3 PROPERTIES FOLDER "Dependencies")