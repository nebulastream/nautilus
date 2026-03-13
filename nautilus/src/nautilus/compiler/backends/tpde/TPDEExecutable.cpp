#include "TPDEExecutable.hpp"

#include "nautilus/exceptions/RuntimeException.hpp"

namespace nautilus::compiler::tpde {

TPDEExecutable::TPDEExecutable(::tpde::elf::AssemblerElf& assembler, ::tpde::SymRef func_sym,
                               const std::unordered_map<std::string, void*>& external_symbols) {
	bool ok = mapper_.map(assembler, [&](std::string_view name) -> void* {
		auto it = external_symbols.find(std::string(name));
		if (it != external_symbols.end()) {
			return it->second;
		}
		return nullptr;
	});

	if (!ok) {
		throw RuntimeException("TPDE ElfMapper: failed to map compiled ELF into executable memory");
	}

	func_ptr_ = mapper_.get_sym_addr(func_sym);
	if (func_ptr_ == nullptr) {
		throw RuntimeException("TPDE ElfMapper: function symbol not found in mapped ELF");
	}
}

} // namespace nautilus::compiler::tpde
