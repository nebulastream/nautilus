#include "nautilus/InlineFunctionRegistry.hpp"

InlineFunctionRegistry& InlineFunctionRegistry::instance() {
    static InlineFunctionRegistry inst; // Meyers singleton pattern for making the registry available during program initialization
    return inst;
}

int InlineFunctionRegistry::addBitcode(void *fn, std::string bitcode) {
    std::lock_guard<std::mutex> lock(mutex_);
    bitcodeRegistry_[fn] = bitcode;
    return 0;
}

int InlineFunctionRegistry::addSymbol(std::string &symbolName, void* ptr) {
	std::lock_guard<std::mutex> lock(mutex_);
	symbolRegistry_[symbolName] = ptr;
	return 0;
}

std::string InlineFunctionRegistry::getBitcode(void *fn) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = bitcodeRegistry_.find(fn);
    return it != bitcodeRegistry_.end() ? it->second : std::string();
}


void *InlineFunctionRegistry::getSymbolAddress(std::string &symbolName) const {
	std::lock_guard<std::mutex> lock(mutex_);
	auto it = symbolRegistry_.find(symbolName);
	return it != symbolRegistry_.end() ? it->second : nullptr;
}

std::unordered_map<std::string, void*> *InlineFunctionRegistry::getSymbolTable() {
	return &symbolRegistry_;
}

int registerBitcodePleaseIgnoreThisThanks(void *fn, const char *bitcodePtr, uint64_t bitcodeLen)
{
    return InlineFunctionRegistry::instance().addBitcode(fn, std::string(bitcodePtr, bitcodeLen));
}

int registerSymbolPleaseIgnoreThisThanks(const char *symbolStringPtr, uint64_t symbolNameLength, void *ptr)
{
	auto symbolName = std::string(symbolStringPtr, symbolNameLength);
	return InlineFunctionRegistry::instance().addSymbol(symbolName, ptr);
}

