# Differential AST fuzzer: generates random well-defined integer programs,
# realizes them as traced Nautilus kernels, and asserts every backend (plus the
# interpreter) agrees with an independent native C++ oracle.
#
# Three executables share the same harness (Harness.hpp) / AST layout
# (Ast.hpp, ByteReader.hpp):
#   * nautilus-fuzz         - coverage-guided libFuzzer binary (Clang only),
#                             built when ENABLE_FUZZING=ON.
#   * nautilus-fuzz-replay  - plain main(), no libFuzzer runtime required, built
#                             when ENABLE_FUZZING or ENABLE_FUZZER_REPLAY is ON.
#                             Runs a deterministic built-in corpus or replays
#                             saved crash inputs from argv.
#   * nautilus-fuzz-seedgen - regenerates corpus/ (see SeedGen.cpp), no
#                             libFuzzer runtime required, built when
#                             ENABLE_FUZZING or ENABLE_FUZZER_REPLAY is ON.
#
# All three are opt-in and kept out of the default ctest run.
#
# Build & run:
#   cmake -DENABLE_FUZZING=ON -DCMAKE_CXX_COMPILER=clang++-21 ..
#   cmake --build . --target nautilus-fuzz
#   ./nautilus/test/fuzz/nautilus-fuzz -dict=../nautilus/test/fuzz/dictionary \
#       -max_total_time=120 ../nautilus/test/fuzz/corpus
#
#   cmake -DENABLE_FUZZER_REPLAY=ON ..
#   cmake --build . --target nautilus-fuzz-replay
#   ./nautilus/test/fuzz/nautilus-fuzz-replay            # built-in smoke corpus
#   ./nautilus/test/fuzz/nautilus-fuzz-replay crash-1234 # replay a finding
#
#   cmake --build . --target nautilus-fuzz-seedgen
#   ./nautilus/test/fuzz/nautilus-fuzz-seedgen ../nautilus/test/fuzz/corpus
#   ./nautilus/test/fuzz/nautilus-fuzz-seedgen --check ../nautilus/test/fuzz/corpus
#
# Long, parallel sessions with crash triage (what the weekly CI sweep runs):
#   ../nautilus/test/fuzz/run-fuzz.sh --fuzzer ./nautilus/test/fuzz/nautilus-fuzz \
#       --work /tmp/fuzz-work --time 3600

function(_nautilus_fuzz_common target)
	target_link_libraries(${target} PRIVATE nautilus)
	if (ENABLE_LOGGING)
		target_link_libraries(${target} PRIVATE spdlog::spdlog)
	endif ()
endfunction()

if (ENABLE_FUZZING)
	add_executable(nautilus-fuzz FuzzMain.cpp)
	_nautilus_fuzz_common(nautilus-fuzz)
	# Link the libFuzzer runtime (provides main + the coverage-driven mutation
	# loop). The rest of the build is instrumented with -fsanitize=fuzzer-no-link,
	# set globally when ENABLE_FUZZING is on.
	target_compile_options(nautilus-fuzz PRIVATE -fsanitize=fuzzer)
	target_link_options(nautilus-fuzz PRIVATE -fsanitize=fuzzer)
endif ()

if (ENABLE_FUZZING OR ENABLE_FUZZER_REPLAY)
	add_executable(nautilus-fuzz-replay ReplayMain.cpp)
	_nautilus_fuzz_common(nautilus-fuzz-replay)

	# Seed-corpus generator (issue #359): a small standalone tool, not linked
	# against libnautilus (it only needs the header-only AST/ByteReader
	# layout), that regenerates corpus/ from Ast.hpp's own Kind/TypeId tables.
	add_executable(nautilus-fuzz-seedgen SeedGen.cpp)

	# The smoke corpus and the committed seeds, for ctest runs of a build that has the replay driver. Not part of a
	# default build: the targets exist only with ENABLE_FUZZER_REPLAY or ENABLE_FUZZING.
	enable_testing()
	add_test(NAME fuzz-replay-smoke COMMAND nautilus-fuzz-replay)
	add_test(NAME fuzz-replay-seeds COMMAND nautilus-fuzz-replay ${CMAKE_CURRENT_SOURCE_DIR}/corpus)
	set_tests_properties(fuzz-replay-smoke fuzz-replay-seeds PROPERTIES LABELS fuzz TIMEOUT 1200)
endif ()

# NIR format fuzzers (M1.6): the binary image loader, the verifier and the text parser. They link only nautilus-nir.
if (ENABLE_FUZZING)
	add_executable(nautilus-nir-fuzz NirImageFuzz.cpp)
	target_link_libraries(nautilus-nir-fuzz PRIVATE nautilus-nir)
	target_compile_options(nautilus-nir-fuzz PRIVATE -fsanitize=fuzzer)
	target_link_options(nautilus-nir-fuzz PRIVATE -fsanitize=fuzzer)
endif ()
if (ENABLE_FUZZING OR ENABLE_FUZZER_REPLAY)
	add_executable(nautilus-nir-fuzz-replay NirReplayMain.cpp)
	target_link_libraries(nautilus-nir-fuzz-replay PRIVATE nautilus-nir)
endif ()
