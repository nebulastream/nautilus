
#include <nautilus/compiler/backends/cpp/CPPCompilerFlags.hpp>

namespace nautilus::compiler::cpp {
CPPCompilerFlags CPPCompilerFlags::create() {
	return CPPCompilerFlags();
}

CPPCompilerFlags CPPCompilerFlags::createDefaultCompilerFlags() {
	auto flags = create();
	flags.addFlag(CXX_VERSION);
	// flags.addFlag(NO_TRIGRAPHS);
	flags.addFlag(FPIC);
	// flags.addFlag(WPARENTHESES_EQUALITY);
#if defined(__SSE4_2__)
	flags.addFlag(SSE_4_2);
#endif
#ifdef __APPLE__
	//  flags.addFlag(std::string("-isysroot ") + std::string(NES_OSX_SYSROOT));
	flags.addFlag(std::string("-DTARGET_OS_IPHONE=0"));
	flags.addFlag(std::string("-DTARGET_OS_SIMULATOR=0"));
#endif
	return flags;
}

void CPPCompilerFlags::enableDebugFlags() {
	addFlag(DEBUGGING);
}

void CPPCompilerFlags::enableOptimizationFlags() {
	addFlag(ALL_OPTIMIZATIONS);
#if !defined(__aarch64__)
	// use -mcpu=native instead of TUNE/ARCH for arm64, below
	// -march=native is supported on Intel Macs clang but not on M1 Macs clang
	addFlag(TUNE);
	addFlag(ARCH);
#endif
#if defined(__aarch64__) && defined(__APPLE__)
	// M1 specific string
	addFlag(M1_CPU);
#endif
#if defined(__aarch64__) && !defined(__APPLE__)
	// generic arm64 cpu
	addFlag(CPU);
#endif
}

std::vector<std::string> CPPCompilerFlags::getFlags() const {
	return compilerFlags;
}

void CPPCompilerFlags::addFlag(const std::string& flag) {
	compilerFlags.emplace_back(flag);
}

} // namespace nautilus::compiler::cpp
