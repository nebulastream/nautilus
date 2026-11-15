# Runs the latency corpus once on one backend and compares the NIR instruction counts with the recorded 0.3 operation
# counts (tools/latency/opcount-report.py, the op-count gate of M3). Used as a ctest (`latency-corpus-opcounts`, builds
# with ENABLE_BENCHMARKS).
#   cmake -DCORPUS=<driver> -DREPORT=<opcount-report.py> -DBASELINE=<corpus.jsonl> -DOUT=<file> -DBACKEND=tbc -P this
execute_process(COMMAND ${CORPUS} 1 "" 0 ${BACKEND} OUTPUT_FILE ${OUT} ERROR_QUIET RESULT_VARIABLE corpusStatus)
if (NOT corpusStatus EQUAL 0)
    message(FATAL_ERROR "the corpus driver failed (${corpusStatus})")
endif ()
execute_process(COMMAND python3 ${REPORT} ${OUT} --baseline ${BASELINE}
        RESULT_VARIABLE status OUTPUT_VARIABLE out ERROR_VARIABLE err)
message("${out}${err}")
file(WRITE ${OUT}.txt "${out}${err}")
if (NOT status EQUAL 0)
    message(FATAL_ERROR "the op-count gate failed")
endif ()
