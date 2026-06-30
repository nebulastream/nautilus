#include "Harness.hpp"
#include <cstddef>
#include <cstdint>

// libFuzzer entry point. The mutation/coverage loop and main() come from the
// libFuzzer runtime (linked via -fsanitize=fuzzer); all the work lives in
// nautilus::fuzz::runOne (Harness.hpp), shared with the replay driver.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
	nautilus::fuzz::runOne(data, size);
	return 0;
}
