
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nautilus/compiler/backends/cpp/File.hpp>
#include <utility>
namespace nautilus::compiler::cpp {

File::File(std::string path) : path(std::move(path)) {
}

std::string File::getPath() const {
	return path;
}

std::shared_ptr<File> File::createFile(const std::string& absoluteFilePath, const std::string& content) {
	// NES_DEBUG("Create File to file://" << absoluteFilePath);
	std::ofstream resultFile(absoluteFilePath, std::ios::trunc | std::ios::out);
	resultFile << content;
	resultFile.flush();
	return std::make_shared<File>(absoluteFilePath);
}

std::string File::read() const {
	const std::lock_guard<std::mutex> fileLock(fileMutex);
	// read source file in
	std::ifstream file(path);
	file.clear();
	std::string sourceCode((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return sourceCode;
}

void File::print() const {
	auto sourceCode = read();
	// NES_DEBUG("Compiler: code \n" << sourceCode);
}

std::mutex& File::getFileMutex() {
	return fileMutex;
}

} // namespace nautilus::compiler::cpp
