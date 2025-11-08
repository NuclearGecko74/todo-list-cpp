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