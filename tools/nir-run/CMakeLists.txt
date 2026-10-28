# nautilus-nir-run: compiles SSA-level NIR text with a NIR backend and runs the `; run` directives of the file
# (docs/plans/nautilus-0.4/16-implementation-plan.md, M4.0).
add_executable(nautilus-nir-run main.cpp)
target_link_libraries(nautilus-nir-run PRIVATE nautilus)

# Native executables (cpp, ...) are called with dyncall, which is built with the tbc backend.
if (TARGET dyncall_s)
    target_compile_definitions(nautilus-nir-run PRIVATE NIR_RUN_DYNCALL)
    target_link_libraries(nautilus-nir-run PRIVATE dyncall_s)
    target_include_directories(nautilus-nir-run PRIVATE ${PROJECT_SOURCE_DIR}/third_party/dyncall/dyncall)
endif ()
