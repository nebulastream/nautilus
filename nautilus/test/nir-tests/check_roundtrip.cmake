# Runs nautilus-nir-opt on a .nir file and requires its output to equal the file (the corpus is in canonical form).
execute_process(COMMAND ${OPT} ${INPUT} OUTPUT_VARIABLE actual ERROR_VARIABLE err RESULT_VARIABLE rc)
if (NOT rc EQUAL 0)
    message(FATAL_ERROR "nautilus-nir-opt failed on ${INPUT}: ${err}")
endif ()
file(READ ${INPUT} expected)
if (NOT actual STREQUAL expected)
    message(FATAL_ERROR "${INPUT} is not in canonical form; nautilus-nir-opt printed:\n${actual}")
endif ()
