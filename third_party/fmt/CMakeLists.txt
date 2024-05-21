if (POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif ()

#add_library(nautilus_fmt STATIC format.cc)

#target_include_directories(nautilus_fmt PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
set_target_properties(nautilus_fmt PROPERTIES EXPORT_NAME nautilus_fmt)

install(TARGETS nautilus_fmt
        EXPORT "${NAUTILUS_EXPORT_SET}"
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")

disable_target_warnings(nautilus_fmt)
