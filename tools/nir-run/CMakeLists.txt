# nautilus-nir-run: compiles SSA-level NIR text with a NIR backend and runs the `; run` directives of the file
# (docs/plans/nautilus-0.4/16-implementation-plan.md, M4.0).
add_executable(nautilus-nir-run main.cpp)
target_link_libraries(nautilus-nir-run PRIVATE nautilus)
