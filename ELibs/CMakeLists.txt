add_subdirectory("GameInput")
add_subdirectory("tracy")

set_property(GLOBAL PROPERTY TRACY_INC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tracy/public/tracy")