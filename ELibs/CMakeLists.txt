
add_subdirectory("GameInput")
add_subdirectory("tracy")

set(ASSIMP_INCLUDE_DIRS "ELibs/assimp/include" "${PROJECT_BINARY_DIR}/ELibs/assimp/include")
include_directories(${ASSIMP_INCLUDE_DIRS})
add_subdirectory("assimp")

set_property(GLOBAL PROPERTY TRACY_INC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tracy/public/tracy")
set_property(GLOBAL PROPERTY ASSIMP_INC_PATH ${ASSIMP_INCLUDE_DIRS})