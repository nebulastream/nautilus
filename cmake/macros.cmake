

function(disable_target_warnings NAME)
    if (MSVC)
        target_compile_options(${NAME} PRIVATE "/W0")
    elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang$"
            OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        target_compile_options(${NAME} PRIVATE "-w")
    endif ()
endfunction()

macro(add_source PROP_NAME SOURCE_FILES)
    set(SOURCE_FILES_ABSOLUTE)
    foreach (it ${SOURCE_FILES})
        get_filename_component(ABSOLUTE_PATH ${it} ABSOLUTE)
        set(SOURCE_FILES_ABSOLUTE ${SOURCE_FILES_ABSOLUTE} ${ABSOLUTE_PATH})
    endforeach ()

    get_property(OLD_PROP_VAL GLOBAL PROPERTY "${PROP_NAME}_SOURCE_PROP")
    set_property(GLOBAL PROPERTY "${PROP_NAME}_SOURCE_PROP" ${SOURCE_FILES_ABSOLUTE} ${OLD_PROP_VAL})
endmacro()

macro(get_source PROP_NAME SOURCE_FILES)
    get_property(SOURCE_FILES_LOCAL GLOBAL PROPERTY "${PROP_NAME}_SOURCE_PROP")
    set(${SOURCE_FILES} ${SOURCE_FILES_LOCAL})
endmacro()

macro(add_source_files)
    set(SOURCE_FILES "${ARGN}")
    list(POP_FRONT SOURCE_FILES TARGET_NAME)
    add_source(${TARGET_NAME} "${SOURCE_FILES}")
endmacro()

macro(add_make_format)
    file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
            ${CMAKE_SOURCE_DIR}/nautilus/*.cpp
            ${CMAKE_SOURCE_DIR}/nautilus/*.h)
    add_custom_target(
            format
            COMMAND clang-format
            -i
            -style=file
            ${ALL_CXX_SOURCE_FILES}
    )
endmacro()


function(download_file url filename)
    message("Download: ${url}")
    file(REMOVE ${filename})
    if (NOT EXISTS ${filename})
        set(CURRENT_ITERATION "0")
        set(MAX_RETRIES "3")
        while (CURRENT_ITERATION LESS MAX_RETRIES)
            # Throws an error if download is inactive for 10s
            file(DOWNLOAD ${url} ${filename} SHOW_PROGRESS TIMEOUT 0 INACTIVITY_TIMEOUT 10 STATUS DOWNLOAD_STATUS)
            # Retrieve download status info
            list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
            list(GET DOWNLOAD_STATUS 1 ERROR_MESSAGE)
            math(EXPR CURRENT_ITERATION "${CURRENT_ITERATION} + 1") # CURRENT_ITERATION++
            if (${STATUS_CODE} EQUAL 0)
                message(STATUS "Download completed successfully")
                break()
            else ()
                message(STATUS "Error occurred during download: ${ERROR_MESSAGE}")
                message(STATUS "Retry attempt ${CURRENT_ITERATION}/${MAX_RETRIES}")
                # Remove created (incomplete) file which failed to get downloaded
                file(REMOVE ${filename})
            endif ()
        endwhile ()
        if (CURRENT_ITERATION EQUAL MAX_RETRIES)
            message(FATAL_ERROR "Aborting: retry attempts exceeded while failing to download ${url}")
        endif ()
    endif ()
endfunction(download_file)

function(get_linux_lsb_release_information)
    find_program(LSB_RELEASE_EXEC lsb_release)
    if (NOT LSB_RELEASE_EXEC)
        message(WARNING "Could not detect lsb_release executable, can not gather required information")
        set(LSB_RELEASE_ID_SHORT "NULL" PARENT_SCOPE)
        set(LSB_RELEASE_VERSION_SHORT "NULL" PARENT_SCOPE)
        set(LSB_RELEASE_CODENAME_SHORT "NULL" PARENT_SCOPE)
    else ()
        execute_process(COMMAND "${LSB_RELEASE_EXEC}" --short --id OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)
        execute_process(COMMAND "${LSB_RELEASE_EXEC}" --short --release OUTPUT_VARIABLE LSB_RELEASE_VERSION_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)
        execute_process(COMMAND "${LSB_RELEASE_EXEC}" --short --codename OUTPUT_VARIABLE LSB_RELEASE_CODENAME_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)

        set(LSB_RELEASE_ID_SHORT "${LSB_RELEASE_ID_SHORT}" PARENT_SCOPE)
        set(LSB_RELEASE_VERSION_SHORT "${LSB_RELEASE_VERSION_SHORT}" PARENT_SCOPE)
        set(LSB_RELEASE_CODENAME_SHORT "${LSB_RELEASE_CODENAME_SHORT}" PARENT_SCOPE)
    endif ()
endfunction()
