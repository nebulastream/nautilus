#pragma once

#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

// use this macro in front of a function to mark it as a candidate function for nautilus to inline
// e.g. NAUTILUS_INLINE int foo(int bar) {...}
// this marker is used by an LLVM pass to detect those functions and extract LLVM IR code for them.
//
// Version counter: bump the `_v0001` suffix whenever any of the following
// change, so ccache / build caches re-run the pass over existing TUs:
//   - the ABI of `registerBitcodePleaseIgnoreThisThanks` /
//     `registerSymbolPleaseIgnoreThisThanks`
//   - the bitcode serialization format produced by
//     `serializeFunctionWithDependencySymbols`
//   - the convention used for the "is_target" function attribute or the
//     hex-encoded symbol names used at JIT time
// The pass uses `starts_with("nautilus_inline")` so multiple versions can
// coexist in one build, but bumping is what forces a recompile.
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

// Anchor function defined in InliningPluginInit.cpp. Taking its address from
// the static below forces that TU (and hence its file-scope static registrar
// that installs the LLVMBackendHooks) to be pulled out of the nautilus-inlining
// static archive whenever a consumer TU includes this header.
int registerInliningHooksPleaseIgnoreThisThanks();

// static registry for storing LLVM-IR bitstrings + symbol addresses for LLVM inlining
// the static registry pattern makes it available during program initialization
class InlineFunctionRegistry {
public:
	static InlineFunctionRegistry& instance(); // get the static singleton instance of the registry

	// Retrieve the bitcode of a target function address. Returns std::nullopt
	// if no bitcode has been registered for `fn`. Returning by value (rather
	// than by reference) keeps the caller safe: the snapshot remains valid
	// even if the registry is mutated concurrently after the call returns.
	std::optional<std::string> getBitcode(void* fn) const;

	// Look up whether the registry contains the bitcode for a function address.
	bool containsFunctionBitcode(void* fn) const;

	// Look up the runtime address of a symbol by name. Returns nullptr if
	// the symbol is not registered.
	void* getSymbolAddress(std::string_view symbolName) const;

	// Returns a snapshot copy of the (name -> address) symbol table. A copy
	// is intentional: the JIT-time callers iterate the table outside the
	// registry's mutex, so a snapshot avoids tearing against shared-object
	// ctors that may still be running `addSymbol`.
	std::unordered_map<std::string, void*> getSymbolTable() const;

private:
	mutable std::mutex mutex_;
	std::unordered_map<void*, std::string> bitcodeRegistry_; // mapping: function runtime address -> LLVM IR bitstring
	std::unordered_map<std::string, void*> symbolRegistry_;  // mapping: symbol name -> runtime address

	// those functions should normally not be used in nautilus, and are used by the llvm pass only
	int addBitcode(void* fn, std::string bitcode);
	int addSymbol(std::string_view symbolName, void* ptr);

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

// Forces InliningPluginInit.cpp to be linked out of the static archive so its
// file-scope registrar runs and installs the LLVM backend hooks. See the
// comment on `registerInliningHooksPleaseIgnoreThisThanks` above.
__attribute__((used)) static const auto inliningHookRegistrationDummyPleaseIgnoreThisThanks =
    &registerInliningHooksPleaseIgnoreThisThanks;
