#ifndef FUNCTIONREGISTRY_H
#define FUNCTIONREGISTRY_H
#include <mutex>
#include <string>
#include <unordered_map>

class InlineFunctionRegistry {
public:
    static InlineFunctionRegistry& instance();

    int addBitcode(void *fn, std::string bitcode);
	int addSymbol(std::string &symbolName, void *ptr);

    std::string getBitcode(void *fn) const;
	void *getSymbolAddress(std::string &symbolName) const;

	std::unordered_map<std::string, void *> *getSymbolTable();
private:
    mutable std::mutex mutex_;
    std::unordered_map<void *, std::string> bitcodeRegistry_;
	std::unordered_map<std::string, void *> symbolRegistry_;

};


int registerBitcodePleaseIgnoreThisThanks(void *fn, const char *bitcodePtr, uint64_t bitcodeLen);
int registerSymbolPleaseIgnoreThisThanks(const char *symbolStringPtr, uint64_t symbolNameLength, void *ptr);


#endif
