add_library(nautilus-std)

target_sources(nautilus-std PRIVATE
	src/bit.cpp
	src/cmath.cpp
	src/cstdlib.cpp
	src/cstring.cpp
	src/iostream.cpp
	src/string.cpp
	src/string_view.cpp
)

target_include_directories(nautilus-std PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include/>)

target_link_libraries(nautilus-std PUBLIC nautilus)

if (ENABLE_TESTS)
	add_subdirectory(test)
endif ()
