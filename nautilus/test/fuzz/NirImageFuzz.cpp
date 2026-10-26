// libFuzzer entry point for the NIR loader, verifier and text parser (see NirFuzzTarget.hpp).
#include "NirFuzzTarget.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
	nautilus::nir::fuzz::runInput(data, size);
	return 0;
}
