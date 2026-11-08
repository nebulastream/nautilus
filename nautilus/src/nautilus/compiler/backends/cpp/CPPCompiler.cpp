
#include "nautilus/compiler/backends/cpp/CPPCompiler.hpp"
#include "nautilus/common/File.hpp"
#include "nautilus/compiler/backends/cpp/CPPCompilerFlags.hpp"
#include "nautilus/compiler/backends/cpp/ExecutablePath.hpp"
#include "nautilus/compiler/backends/cpp/SharedLibrary.hpp"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <spawn.h>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <system_error>
#include <unistd.h>

#if defined(__APPLE__)
#include <crt_externs.h>
#define NAUTILUS_ENVIRON (*_NSGetEnviron())
#else
extern char** environ;
#define NAUTILUS_ENVIRON environ
#endif

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

	// Neither file is needed once the library is loaded (dlopen keeps its own
	// mapping), and both must also go when compilation fails; otherwise every
	// compile leaves its source in the temp directory for good.
	struct RemoveOnExit {
		const std::string& source;
		const std::string& library;
		~RemoveOnExit() {
			std::error_code ignored;
			std::filesystem::remove(source, ignored);
			std::filesystem::remove(library, ignored);
		}
	} removeOnExit {sourceFileName, libraryFileName};

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
	// compiler, assembler and linker run concurrently, connected by pipes, instead of one after the other through
	// temporary files
	compilationFlags.addFlag("-pipe");

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
	return SharedLibrary::load(libraryFileName);
}

void CPPCompiler::compileSharedLib(CPPCompilerFlags flags, std::shared_ptr<common::File> sourceFile,
                                   std::string) const {
	// lock file, such that no one can operate on the file at the same time
	const std::lock_guard<std::mutex> fileLock(sourceFile->getFileMutex());

	// The arguments as a shell would have split them: a flag may hold several words (`-std=c++20 -stdlib=libc++`).
	// Starting the compiler directly saves the `sh` that popen runs between this process and `cc`, about 2 ms of a
	// compile that takes 30.
	std::vector<std::string> arguments {"cc"};
	for (const auto& flag : flags.getFlags()) {
		std::istringstream words(flag);
		for (std::string word; words >> word;) {
			arguments.push_back(std::move(word));
		}
	}
	std::vector<char*> argv;
	argv.reserve(arguments.size() + 1);
	for (auto& argument : arguments) {
		argv.push_back(argument.data());
	}
	argv.push_back(nullptr);

	// Standard output and standard error of the compiler go through one pipe, to be able to report its messages
	// The pipe is close-on-exec: another thread compiling at the same time must not hand our write end to its compiler,
	// or the read below would not see the end of the output before that one had finished. dup2 clears the flag on the
	// two descriptors the compiler gets.
	int output[2];
#if defined(__linux__)
	const bool piped = ::pipe2(output, O_CLOEXEC) == 0;
#else
	const bool piped = ::pipe(output) == 0;
	if (piped) {
		::fcntl(output[0], F_SETFD, FD_CLOEXEC);
		::fcntl(output[1], F_SETFD, FD_CLOEXEC);
	}
#endif
	if (!piped) {
		throw std::runtime_error("cannot start the compiler: " + std::string(std::strerror(errno)));
	}
	posix_spawn_file_actions_t actions;
	posix_spawn_file_actions_init(&actions);
	posix_spawn_file_actions_adddup2(&actions, output[1], STDOUT_FILENO);
	posix_spawn_file_actions_adddup2(&actions, output[1], STDERR_FILENO);
	pid_t child = 0;
	const int spawned = ::posix_spawnp(&child, "cc", &actions, nullptr, argv.data(), NAUTILUS_ENVIRON);
	posix_spawn_file_actions_destroy(&actions);
	::close(output[1]);
	if (spawned != 0) {
		::close(output[0]);
		throw std::runtime_error("cannot start the compiler 'cc': " + std::string(std::strerror(spawned)));
	}

	// Collecting the output of the compiler
	std::string messages;
	char buffer[8192];
	for (;;) {
		const auto count = ::read(output[0], buffer, sizeof(buffer));
		if (count > 0) {
			messages.append(buffer, static_cast<size_t>(count));
		} else if (count == 0 || errno != EINTR) {
			break;
		}
	}
	::close(output[0]);

	int status = 0;
	while (::waitpid(child, &status, 0) < 0) {
		if (errno != EINTR) {
			throw std::runtime_error("cannot wait for the compiler: " + std::string(std::strerror(errno)));
		}
	}

	// If the compilation didn't return with 0, we throw an exception containing
	// the compiler output
	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
		throw std::runtime_error(messages);
	}
}

} // namespace nautilus::compiler::cpp
