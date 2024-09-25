# Create files and directories if non existing in given Context path
function(create_non_existing FILE_PATHS CONTEXT CONTEXT_PRETTY_NAME)
    # Iterate over each file path
    foreach(FILE_PATH ${FILE_PATHS})
      # Get the directory path of the file
      get_filename_component(DIR_PATH ${FILE_PATH} DIRECTORY)
      
      # Check if the directory does not exist
      if(NOT EXISTS "${CONTEXT}/${DIR_PATH}")
        # Create the directory
        file(MAKE_DIRECTORY ${DIR_PATH})
        message("Created new dir : '${CONTEXT_PRETTY_NAME}/${DIR_PATH}'")
      endif()
    
      # Check if the file does not exist
      if(NOT EXISTS "${CONTEXT}/${FILE_PATH}")
        # Create the file
        GET_FILENAME_COMPONENT(FILE_EXT "${FILE_PATH}" EXT)
        if("${FILE_EXT}" STREQUAL ".h")
            file(APPEND ${FILE_PATH} "\#pragma once")
        elseif("${FILE_EXT}" STREQUAL ".cpp")
            string(FIND "${FILE_PATH}" "/Private/" PRIVATE_PATH_POS)
            if(PRIVATE_PATH_POS EQUAL -1)
                # Cannot fin a generic private path to deduce an #include public path
                file(APPEND ${FILE_PATH} "")
            else()
                # Did found a generic private path to deduce an #include public path
                GET_FILENAME_COMPONENT(FILE_NAME "${FILE_PATH}" NAME_WLE)
                string(REGEX REPLACE "^(.*[/\\\\]Private[/\\\\])" "" CLEANED_DIR_PATH ${DIR_PATH})
                file(APPEND ${FILE_PATH} "\#include \"${CLEANED_DIR_PATH}/${FILE_NAME}.h\"")
            endif()
        else()
            file(APPEND ${FILE_PATH} "")
        endif()
        message("Created new file: '${CONTEXT_PRETTY_NAME}/${FILE_PATH}'")
      endif()
    endforeach()
endfunction()
