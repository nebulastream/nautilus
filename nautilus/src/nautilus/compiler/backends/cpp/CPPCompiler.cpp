
#include "nautilus/compiler/backends/cpp/CPPCompiler.hpp"
#include "nautilus/common/File.hpp"
#include "nautilus/compiler/backends/cpp/CPPCompilerFlags.hpp"
#include "nautilus/compiler/backends/cpp/ExecutablePath.hpp"
#include "nautilus/compiler/backends/cpp/SharedLibrary.hpp"
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>

using namespace std::string_literals;
namespace nautilus::compiler::cpp {

std::shared_ptr<CPPCompiler> CPPCompiler::create() {
	return std::make_shared<CPPCompiler>();
}

CPPCompiler::CPPCompiler() : runtimePathConfig(ExecutablePath::loadRuntimePathConfig()) {
}

CPPCompiler::~CPPCompiler() noexcept = default;

SharedLibraryPtr CPPCompiler::compile(const std::string& identifier, const std::string& code) const {
	// Timer timer("CPPCompiler");
	// timer.start();
	std::string fileName = (std::filesystem::temp_directory_path() / identifier);
	auto sourceFileName = fileName + ".cpp";
	auto libraryFileName = fileName +
#ifdef __linux__
	                       ".so";
#elif defined(__APPLE__)
	                       ".dylib";
#else
#error "Unknown platform"
#endif

	auto file = common::File::createFile(sourceFileName, code);
	auto compilationFlags = CPPCompilerFlags::createDefaultCompilerFlags();
	// if (request->enableOptimizations()) {
	//  NES_DEBUG("Compile with optimizations.");
	//     compilationFlags.enableOptimizationFlags();
	// }
	// if (request->enableDebugging()) {
	//     NES_DEBUG("Compile with debugging.");
	//     compilationFlags.enableDebugFlags();
	//    format->formatFile(file);
	//     file->print();
	// }
	// if (request->enableCompilationProfiling()) {
	//     compilationFlags.addFlag(CPPCompilerFlags::TRACE_COMPILATION_TIME);
	//     NES_DEBUG("Compilation Time tracing is activated open:
	//     chrome://tracing/");
	// }
	compilationFlags.addFlag("-shared");
	compilationFlags.addFlag("-g");

	// add header
	for (auto libPaths : runtimePathConfig.libPaths) {
		compilationFlags.addFlag(std::string("-L") + libPaths);
	}
	// add libs
	for (auto libs : runtimePathConfig.libs) {
		compilationFlags.addFlag(libs);
	}
	// add header
	for (auto includePath : runtimePathConfig.includePaths) {
		compilationFlags.addFlag("-I" + includePath);
	}

	compilationFlags.addFlag("-o" + libraryFileName);

	compilationFlags.addFlag(sourceFileName);

	compileSharedLib(compilationFlags, file, libraryFileName);

	// load shared lib
	auto sharedLibrary = SharedLibrary::load(libraryFileName);

	// timer.pause();
	/* if (!request->enableDebugging()) {
	     std::filesystem::remove(sourceFileName);
	     std::filesystem::remove(libraryFileName);
	 }*/
	// NES_INFO("CPPCompiler Runtime: " << (double) timer.getRuntime() / (double)
	// 1000000 << "ms");// print runtime
	std::filesystem::remove(libraryFileName);
	return sharedLibrary;
}

void CPPCompiler::compileSharedLib(CPPCompilerFlags flags, std::shared_ptr<common::File> sourceFile,
                                   std::string) const {
	// lock file, such that no one can operate on the file at the same time
	const std::lock_guard<std::mutex> fileLock(sourceFile->getFileMutex());

	std::stringstream compilerCall;
	compilerCall << " cc ";
	for (const auto& arg : flags.getFlags()) {
		compilerCall << arg << " ";
	}
	// NES_ERROR("Compiler: compile with: '" << compilerCall.str() << "'");
	//  Creating a pointer to an open stream and a buffer, to read the output of
	//  the compiler
	FILE* fp = nullptr;
	char buffer[8192];

	// Redirecting stderr to stdout, to be able to read error messages
	compilerCall << " 2>&1";

	// Calling the compiler in a new process
	fp = popen(compilerCall.str().c_str(), "r");

	if (fp == nullptr) {
		// NES_ERROR("Compiler: failed to run command\n");
		return;
	}

	// Collecting the output of the compiler to a string stream
	std::ostringstream strstream;
	while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
		strstream << buffer;
	}

	// Closing the stream, which also gives us the exit status of the compiler
	// call
	auto ret = pclose(fp);

	// If the compilation didn't return with 0, we throw an exception containing
	// the compiler output
	if (ret != 0) {
		// NES_ERROR("Compiler: compilation of " << libraryFileName << " failed.");
		throw std::runtime_error(strstream.str());
	}
}

} // namespace nautilus::compiler::cpp
