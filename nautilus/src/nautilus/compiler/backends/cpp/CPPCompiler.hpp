#pragma once

#include "nautilus/compiler/backends/cpp/ExecutablePath.hpp"
#include <filesystem>
#include <vector>

namespace nautilus::common {
class File;
}

namespace nautilus::compiler::cpp {
class CPPCompilerFlags;

class ClangFormat;

class SharedLibrary;

/**
 * @brief A @LanguageCompiler for C++.
 */
class CPPCompiler {
public:
	CPPCompiler();

	~CPPCompiler() noexcept;

	/**
	 * @brief Creates a new instance of the cpp compiler.
	 * @return std::shared_ptr<LanguageCompiler>
	 */
	static std::shared_ptr<CPPCompiler> create();

	/**
	 * @brief Handles a compilation request. Implementations have to be thread safe.
	 * @param request CompilationRequest
	 * @return CompilationResult
	 */
	[[nodiscard]] std::shared_ptr<SharedLibrary> compile(const std::string& identifier, const std::string& code) const;

private:
	/**
	 * @brief Compiles a source code file to a shared lib.
	 * @param flags compilation flags
	 * @param sourceFile the source code file
	 * @param libraryFileName the target lib name
	 */
	void compileSharedLib(CPPCompilerFlags flags, std::shared_ptr<common::File> sourceFile, std::string libraryFileName) const;

	ExecutablePath::RuntimePathConfig runtimePathConfig;
};

} // namespace nautilus::compiler::cpp
