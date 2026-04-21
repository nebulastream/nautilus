#include "nautilus/inline.hpp"

InlineFunctionRegistry& InlineFunctionRegistry::instance() {
	static InlineFunctionRegistry
	    inst; // Meyers singleton pattern for making the registry available during program initialization
	return inst;
}

int InlineFunctionRegistry::addBitcode(void* fn, std::string bitcode) {
	std::lock_guard<std::mutex> lock(mutex_);
	bitcodeRegistry_[fn] = std::move(bitcode);
	return 0;
}

int InlineFunctionRegistry::addSymbol(std::string_view symbolName, void* ptr) {
	std::lock_guard<std::mutex> lock(mutex_);
	symbolRegistry_[std::string(symbolName)] = ptr;
	return 0;
}

std::optional<std::string> InlineFunctionRegistry::getBitcode(void* fn) const {
	std::lock_guard<std::mutex> lock(mutex_);
	auto it = bitcodeRegistry_.find(fn);
	if (it == bitcodeRegistry_.end()) {
		return std::nullopt;
	}
	return it->second;
}

bool InlineFunctionRegistry::containsFunctionBitcode(void* fn) const {
	std::lock_guard<std::mutex> lock(mutex_);
	return bitcodeRegistry_.contains(fn);
}

void* InlineFunctionRegistry::getSymbolAddress(std::string_view symbolName) const {
	std::lock_guard<std::mutex> lock(mutex_);
	// Heterogeneous lookup is not available for std::unordered_map in C++20,
	// so materialize the name into a std::string for the find() call.
	auto it = symbolRegistry_.find(std::string(symbolName));
	return it != symbolRegistry_.end() ? it->second : nullptr;
}

std::unordered_map<std::string, void*> InlineFunctionRegistry::getSymbolTable() const {
	std::lock_guard<std::mutex> lock(mutex_);
	return symbolRegistry_;
}

int registerBitcodePleaseIgnoreThisThanks(void* fn, const char* bitcodePtr, uint64_t bitcodeLen) {
	return InlineFunctionRegistry::instance().addBitcode(fn, std::string(bitcodePtr, bitcodeLen));
}

int registerSymbolPleaseIgnoreThisThanks(const char* symbolStringPtr, uint64_t symbolNameLength, void* ptr) {
	return InlineFunctionRegistry::instance().addSymbol(std::string_view(symbolStringPtr, symbolNameLength), ptr);
}
