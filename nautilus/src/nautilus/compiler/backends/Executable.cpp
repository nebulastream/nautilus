#include "nautilus/Executable.hpp"

namespace nautilus::compiler {
Executable::~Executable() = default;

void Executable::GenericInvocable::setGeneratedFiles(const std::map<std::string, std::string>& generatedFiles) {
	this->generatedFiles = generatedFiles;
}

void Executable::setGeneratedFiles(const std::map<std::string, std::string>& generatedFiles) {
	this->generatedFiles = generatedFiles;
}

const std::string_view Executable::getGeneratedFile(std::string_view key) const {
	auto it = generatedFiles.find(std::string(key));
	if (it != generatedFiles.end()) {
		return it->second;
	}
	return "";
}
} // namespace nautilus::compiler
