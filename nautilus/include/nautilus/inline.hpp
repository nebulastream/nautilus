#ifndef INLINE_HPP
#define INLINE_HPP
#include "InlineFunctionRegistry.hpp"

//This needs to exist here, in order to inject the otherwise unused registerFunction declaration into the LLVM IR
__attribute__((used)) static const auto bitcodeRegistrationFuncPtrDummyPleaseIgnoreThisThanks = &registerBitcodePleaseIgnoreThisThanks;

//This needs to exist here, in order to inject the otherwise unused registerSymbol declaration into the LLVM IR
__attribute__((used)) static const auto symbolRegistrationFuncPtrDummyPleaseIgnoreThisThanks = &registerSymbolPleaseIgnoreThisThanks;

// increment version counter whenever the pass is updated to force re-compilation
#define NAUT_INLINE __attribute__((annotate("naut_inline_v39")))

#endif
