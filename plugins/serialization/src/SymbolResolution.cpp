#include "nautilus/serialization/IRSerialization.hpp"
#include <dlfcn.h>
#include <random>

namespace nautilus::serialization {

SerializationException::SerializationException(std::string message)
    : std::runtime_error("IR serialization: " + std::move(message)) {
}

std::optional<std::string> DynamicLinkerSymbols::nameOf(const void* address) const {
	Dl_info info {};
	// Only an exact match names the address: dladdr also answers for an
	// address in the middle of a function, which would resolve back to the
	// function's start on the other side.
	if (address == nullptr || dladdr(address, &info) == 0 || info.dli_sname == nullptr || info.dli_saddr != address) {
		return std::nullopt;
	}
	return std::string(info.dli_sname);
}

void* DynamicLinkerSymbols::resolve(std::string_view symbol) const {
	const std::string name(symbol);
	return dlsym(RTLD_DEFAULT, name.c_str());
}

SymbolTable::SymbolTable(const SymbolNamer* fallbackNamer, const SymbolResolver* fallbackResolver)
    : fallbackNamer_(fallbackNamer), fallbackResolver_(fallbackResolver) {
}

void SymbolTable::add(std::string name, const void* address) {
	if (address == nullptr) {
		throw SerializationException("cannot register symbol '" + name + "' for a null address");
	}
	if (byName_.contains(name) || byAddress_.contains(address)) {
		throw SerializationException("symbol '" + name + "' or its address is already registered");
	}
	byAddress_.emplace(address, name);
	byName_.emplace(std::move(name), address);
}

std::optional<std::string> SymbolTable::nameOf(const void* address) const {
	if (const auto it = byAddress_.find(address); it != byAddress_.end()) {
		return it->second;
	}
	return fallbackNamer_ != nullptr ? fallbackNamer_->nameOf(address) : std::nullopt;
}

void* SymbolTable::resolve(std::string_view symbol) const {
	if (const auto it = byName_.find(std::string(symbol)); it != byName_.end()) {
		return const_cast<void*>(it->second);
	}
	return fallbackResolver_ != nullptr ? fallbackResolver_->resolve(symbol) : nullptr;
}

uint64_t currentProcessToken() {
	// Random rather than derived from the pid: a pid is reused, and a buffer
	// written by a dead process must never be mistaken for one of ours.
	static const uint64_t token = [] {
		std::random_device device;
		uint64_t value = 0;
		while (value == 0) {
			value = (static_cast<uint64_t>(device()) << 32) ^ device();
		}
		return value;
	}();
	return token;
}

} // namespace nautilus::serialization
