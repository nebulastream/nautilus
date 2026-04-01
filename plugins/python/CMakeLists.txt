find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
include(FetchContent)

FetchContent_Declare(
	pybind11
	GIT_REPOSITORY https://github.com/pybind/pybind11.git
	GIT_TAG v2.13.6
)
FetchContent_MakeAvailable(pybind11)

# Ensure nautilus and its dependencies are compiled with -fPIC for shared library linking
set_target_properties(nautilus PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Also set PIC on third-party dependencies that nautilus links against
foreach(dep fmt spdlog dyncall_s dyncallback_s backward_interface)
	if(TARGET ${dep})
		set_target_properties(${dep} PROPERTIES POSITION_INDEPENDENT_CODE ON)
	endif()
endforeach()

pybind11_add_module(nautilus_native
	src/nautilus_module.cpp
	src/val_bindings.cpp
	src/engine_bindings.cpp
)

target_link_libraries(nautilus_native PRIVATE nautilus)
target_include_directories(nautilus_native PRIVATE
	${CMAKE_SOURCE_DIR}/nautilus/include
	${CMAKE_SOURCE_DIR}/nautilus/src
)

# Copy the Python package next to the compiled module so `import nautilus` works
add_custom_command(TARGET nautilus_native POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_SOURCE_DIR}/nautilus
		$<TARGET_FILE_DIR:nautilus_native>/nautilus
)
