set_property(GLOBAL PROPERTY GAMEINPUT_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/x64")
set_property(GLOBAL PROPERTY GAMEINPUT_INC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Inc")


get_property(GAMEINPUT_LIB_PATH GLOBAL PROPERTY GAMEINPUT_LIB_PATH)
find_library(GI_LIB NAMES GameInput PATHS ${GAMEINPUT_LIB_PATH})

# If the library is found
if(GI_LIB)
    message("Library found at ${GI_LIB}")
    # Set the property in the global scope
	set_property(GLOBAL PROPERTY GAMEINPUT_LIB ${GI_LIB})

else()
    message(FATAL_ERROR "Library not found")
endif()
