#pragma once

#include <mutex>
#include <string>
#include <unordered_map>

// increment version counter whenever the pass is updated to force re-compilation
#define NAUT_INLINE __attribute__((annotate("naut_inline_v49")))

class InlineFunctionRegistry {
public:
	static InlineFunctionRegistry& instance();

	int addBitcode(void* fn, std::string bitcode);
	int addSymbol(std::string& symbolName, void* ptr);

	std::string getBitcode(void* fn) const;
	bool containsFunction(void* fn) const;
	void* getSymbolAddress(std::string& symbolName) const;

	std::unordered_map<std::string, void*>* getSymbolTable();

private:
	mutable std::mutex mutex_;
	std::unordered_map<void*, std::string> bitcodeRegistry_;
	std::unordered_map<std::string, void*> symbolRegistry_;
};

int registerBitcodePleaseIgnoreThisThanks(void* fn, const char* bitcodePtr, uint64_t bitcodeLen);
int registerSymbolPleaseIgnoreThisThanks(const char* symbolStringPtr, uint64_t symbolNameLength, void* ptr);

// This needs to exist here, in order to inject the otherwise unused registerFunction declaration into the LLVM IR of
// the target module
__attribute__((used)) static const auto bitcodeRegistrationFuncPtrDummyPleaseIgnoreThisThanks =
    &registerBitcodePleaseIgnoreThisThanks;

// This needs to exist here, in order to inject the otherwise unused registerSymbol declaration into the LLVM IR of the
// target module
__attribute__((used)) static const auto symbolRegistrationFuncPtrDummyPleaseIgnoreThisThanks =
    &registerSymbolPleaseIgnoreThisThanks;
