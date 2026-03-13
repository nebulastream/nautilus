#pragma once

#include "nautilus/Executable.hpp"
#include "tpde/AssemblerElf.hpp"
#include "tpde/ElfMapper.hpp"
#include <string>
#include <unordered_map>

namespace nautilus::compiler::tpde {

/// Wraps TPDE's ElfMapper to satisfy the Nautilus Executable interface.
/// Holds the mapped executable memory region alive for the object's lifetime.
class TPDEExecutable : public Executable {
public:
	/// Map the ELF output from the assembler into executable memory.
	///
	/// @param assembler  Finished AssemblerElf whose object file will be loaded.
	/// @param func_sym   Symbol reference for the compiled function entry point.
	/// @param external_symbols  Map from symbol name to runtime address for
	///                          external functions (ProxyCallOp targets).
	TPDEExecutable(::tpde::elf::AssemblerElf& assembler, ::tpde::SymRef func_sym,
	               const std::unordered_map<std::string, void*>& external_symbols);

	~TPDEExecutable() override = default;

	[[nodiscard]] void* getInvocableFunctionPtr(const std::string& /*member*/) override {
		return func_ptr_;
	}

	[[nodiscard]] bool hasInvocableFunctionPtr() override { return func_ptr_ != nullptr; }

private:
	::tpde::elf::ElfMapper mapper_;
	void* func_ptr_{nullptr};
};

} // namespace nautilus::compiler::tpde
