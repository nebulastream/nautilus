
#pragma once

#include "nautilus/compiler/Frame.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <variant>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__)
#include <asmjit/x86.h>
#else
#include <asmjit/a64.h>
#endif

namespace nautilus::compiler::asmjit {

// Architecture-specific Compiler + register types used by both the
// X64/A64 lowering providers and the intrinsic-plugin framework.
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__)
using AsmJitCompiler = ::asmjit::x86::Compiler;
using AsmReg = std::variant<::asmjit::x86::Gp, ::asmjit::x86::Xmm>;
#else
using AsmJitCompiler = ::asmjit::a64::Compiler;
using AsmReg = std::variant<::asmjit::a64::Gp, ::asmjit::a64::Vec>;
#endif

using RegisterFrame = Frame<ir::OperationIdentifier, AsmReg>;

} // namespace nautilus::compiler::asmjit
