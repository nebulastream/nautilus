cmake_minimum_required(VERSION 3.22.1)
include(CMakeDependentOption)


option(ENABLE_LOGGING "Enable logging support" ON)
option(ENABLE_STACKTRACE "Enable stacktrace support" ON)
option(ENABLE_TESTS "Enable tests" ON)
option(ENABLE_COMPILER "Enables tracing and backend compilers" ON)
cmake_dependent_option(ENABLE_TRACING "Enable the tracing" ON "ENABLE_COMPILER" OFF)
cmake_dependent_option(ENABLE_MLIR_BACKEND "Enable the MLIR compiler backend" ON "ENABLE_COMPILER" OFF)
cmake_dependent_option(NAUTILUS_DOWNLOAD_MLIR "USE_PRE_BUILD_MLIR" ON "ENABLE_MLIR_BACKEND" OFF)
cmake_dependent_option(ENABLE_C_BACKEND "Enable the C compiler backend" ON "ENABLE_COMPILER" OFF)
cmake_dependent_option(ENABLE_BC_BACKEND "Enable the bytecode interpreter backend" ON "ENABLE_COMPILER" OFF)
cmake_dependent_option(ENABLE_ASMJIT_BACKEND "Enable the asmjit interpreter backend" OFF "ENABLE_COMPILER" OFF)

if (ENABLE_MLIR_BACKEND)
    include(cmake/ImportMLIR.cmake)
endif ()

project(nautilus)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CATCH_ENABLE_REPRODUCIBLE_BUILD OFF CACHE INTERNAL "Turn off tests")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Werror=extra -Werror=all -fpermissive -fPIC -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include(cmake/macros.cmake)

Include(ExternalProject)
Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.2 # or a later release
)
FetchContent_MakeAvailable(Catch2)

# We use the dyncall lib to call arbitrary runtime function.
# Set the path to the dyncall directory
set(dyncall_dir "${CMAKE_CURRENT_BINARY_DIR}/third_party/dyncall")

# Create the include directory for dyncall
file(MAKE_DIRECTORY ${dyncall_dir}/include)

# Download and build the external project
ExternalProject_Add(dyncallProject
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/dyncall
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> # Specify installation directory
        # BUILD_IN_SOURCE true # Build the project in the source directory
        BUILD_BYPRODUCTS <INSTALL_DIR>/lib/libdyncall_s.a # Specify path to the library file that will be built
        INSTALL_DIR ${dyncall_dir} # Install dyncall to the specified directory
        UPDATE_DISCONNECTED 1
)

# Create a static imported library for the external project
add_library(dyncall STATIC IMPORTED)
add_dependencies(dyncall dyncallProject)

# Get the installation directory of the external project
ExternalProject_Get_Property(dyncallProject install_dir)

# Add the include directory of the external project to the interface of extLib
target_include_directories(dyncall INTERFACE ${install_dir}/include)

# Set the imported location of the external library to the installation directory
set_target_properties(dyncall PROPERTIES IMPORTED_LOCATION ${install_dir}/lib/libdyncall_s.a)


add_subdirectory(third_party)
add_subdirectory(nautilus)
add_make_format()

if (ENABLE_STACKTRACE)
    # Also requires one of: libbfd (gnu binutils), libdwarf, libdw (elfutils)
    add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/backward-cpp)

    # Add Backward to your target (either Backward::Interface, Backward::Object, or Backward::Backward)
    target_link_libraries(nautilus PRIVATE Backward::Interface)
endif ()

ExternalProject_Add(spdlog
        PREFIX spdlog
        SOURCE_DIR ${PROJECT_SOURCE_DIR}/third_party/spdlog
        CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/spdlog
        -DSPDLOG_BUILD_SHARED=OFF
        -DSPDLOG_COMPILED_LIB=ON
)

add_dependencies(nautilus spdlog)
target_include_directories(nautilus PRIVATE ${CMAKE_BINARY_DIR}/spdlog/include/)


if (ENABLE_ASMJIT_BACKEND)
    set(ASMJIT_STATIC ON)
    set(ASMJIT_BUILD_RELEASE ON)
    set(ASMJIT_NO_FOREIGN ON)
    set(ASMJIT_NO_DEPRECATED ON)
    add_subdirectory(third_party/asmjit)
    add_dependencies(nautilus asmjit)
    target_link_libraries(nautilus PRIVATE asmjit)
    target_compile_options(asmjit PUBLIC -Wno-deprecated-anon-enum-enum-conversion -Wno-nested-anon-types -Wno-gnu-anonymous-struct)
endif ()
