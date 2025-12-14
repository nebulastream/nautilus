#pragma once

#include <mutex>
#include <string>
#include <unordered_map>

// use this macro in front of a function to mark it as a candidate function for nautilus to inline
// e.g. NAUTILUS_INLINE int foo(int bar) {...}
// this marker is used by an LLVM pass to detect those functions and extract LLVM IR code for them
// increment version counter whenever the pass is updated to force re-compilation
#if defined(__clang__)
#define NAUTILUS_INLINE __attribute__((annotate("nautilus_inline_v0001")))
#else
// GCC and other compilers don't support the annotate attribute, so we make it a no-op
// The inlining pass only works with Clang anyway
#define NAUTILUS_INLINE
#endif

// !!! everything below is for nautilus internals and not relevant for projects that use nautilus

int registerBitcodePleaseIgnoreThisThanks(void* fn, const char* bitcodePtr, uint64_t bitcodeLen);
int registerSymbolPleaseIgnoreThisThanks(const char* symbolStringPtr, uint64_t symbolNameLength, void* ptr);

// static registry for storing LLVM-IR bitstrings + symbol addresses for LLVM inlining
// the static registry pattern makes it available during program initialization
class InlineFunctionRegistry {
public:
	static InlineFunctionRegistry& instance(); // get the static singleton instance of the registry

	const std::string& getBitcode(void* fn) const; // retrieve the bitcode of a target function address
	bool
	containsFunctionBitcode(void* fn) const; // look up whether the registry contains the bitcode for a function address
	void* getSymbolAddress(std::string& symbolName) const; // look up the runtime address of a symbol by name

	const std::unordered_map<std::string, void*>& getSymbolTable();

private:
	mutable std::mutex mutex_;
	std::unordered_map<void*, std::string> bitcodeRegistry_; // mapping: function runtime address -> LLVM IR bitstring
	std::unordered_map<std::string, void*> symbolRegistry_;  // mapping: symbol name -> runtime address

	// those functions should normally not be used in nautilus, and are used by the llvm pass only
	int addBitcode(void* fn, std::string bitcode);
	int addSymbol(std::string& symbolName, void* ptr);

	friend int ::registerBitcodePleaseIgnoreThisThanks(void* fn, const char* bitcodePtr, uint64_t bitcodeLen);
	friend int ::registerSymbolPleaseIgnoreThisThanks(const char* symbolStringPtr, uint64_t symbolNameLength,
	                                                  void* ptr);
};

// This needs to exist here, in order to inject the otherwise unused registerFunction declaration into the LLVM IR of
// the target module
__attribute__((used)) static const auto bitcodeRegistrationFuncPtrDummyPleaseIgnoreThisThanks =
    &registerBitcodePleaseIgnoreThisThanks;

// This needs to exist here, in order to inject the otherwise unused registerSymbol declaration into the LLVM IR of the
// target module
__attribute__((used)) static const auto symbolRegistrationFuncPtrDummyPleaseIgnoreThisThanks =
    &registerSymbolPleaseIgnoreThisThanks;
