#include "nautilus/CompilableFunction.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/compiler/backends/cpp/CPPCompiler.hpp"
#include "nautilus/compiler/backends/cpp/SharedLibrary.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/nir/Parser.hpp"
#include "nautilus/tracing/TraceContext.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include <atomic>
#include <catch2/catch_all.hpp>
#include <filesystem>
#include <functional>
#include <list>
#include <string>
#include <thread>
#include <vector>
#if defined(__unix__) || defined(__APPLE__)
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace nautilus::engine {

namespace {

val<int32_t> cppTempFileAddOne(val<int32_t> x) {
	return x + 1;
}

} // namespace

// The cpp backend writes its generated source and the shared library into the
// temp directory under the compilation unit ID. Both must be gone once the
// module is compiled, or every compile leaves files behind for good.
TEST_CASE("CPP backend removes its temporary files after compiling") {
	Options options;
	options.setOption("engine.backend", std::string("cpp"));
	auto engine = NautilusEngine(options);
	auto module = engine.createModule();
	module.registerFunction("add_one", cppTempFileAddOne);
	auto compiled = module.compile();
	REQUIRE(compiled.getFunction<int32_t(int32_t)>("add_one")(41) == 42);

	const auto* unitId = compiled.getStatistics()->find("compilation.unitId");
	REQUIRE(unitId != nullptr);
	// The file names carry a suffix that makes them unique, so look for the prefix.
	const std::string prefix = "nautilus_" + std::get<std::string>(*unitId);
	for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::temp_directory_path())) {
		INFO("left behind: " << entry.path());
		CHECK(entry.path().filename().string().rfind(prefix, 0) != 0);
	}
}

namespace {

constexpr int Threads = 8;
constexpr int PerThread = 6;

// `value` as the one function of a module, as the cpp backend compiles it.
std::string sourceReturning(int value) {
	return "extern \"C\" int execute() { return " + std::to_string(value) + "; }\n";
}

void runThreads(const std::function<int(int)>& body) {
	std::atomic<int> errors {0};
	std::vector<std::thread> threads;
	for (int t = 0; t < Threads; ++t) {
		threads.emplace_back([&, t] {
			try {
				errors += body(t);
			} catch (const std::exception& e) {
				UNSCOPED_INFO(e.what());
				++errors;
			}
		});
	}
	for (auto& thread : threads) {
		thread.join();
	}
	REQUIRE(errors.load() == 0);
}

// A compilation unit graph without an id: what the backend tests and the benchmark create. Its files were named
// /tmp/nautilus_.* for every compile, so concurrent compiles overwrote the source of each other, removed each other's
// library, or loaded the library of another compile.
std::shared_ptr<compiler::ir::IRGraph> traceReturning(int32_t value) {
	common::Arena arena;
	std::list<compiler::CompilableFunction> functions;
	functions.emplace_back("execute", [value] {
		auto& constant = tracing::traceConstant(Type::i32, tracing::createConstLiteral(value));
		tracing::traceReturnOperation(Type::i32, constant);
	});
	auto module = tracing::TraceContext::Trace(functions, engine::Options {}, arena);
	return tracing::TraceToIRConversionPhase().apply(std::shared_ptr<tracing::TraceModule>(std::move(module)));
}

} // namespace

TEST_CASE("CPP compiler names the files of every compile uniquely") {
	// the identifier is empty, or the same for every compile
	for (const char* identifier : {"", "nautilus_same"}) {
		runThreads([&](int t) {
			int errors = 0;
			for (int i = 0; i < PerThread; ++i) {
				const int value = t * PerThread + i;
				auto library = compiler::cpp::CPPCompiler::create()->compile(identifier, sourceReturning(value));
				auto* execute = reinterpret_cast<int (*)()>(library->getInvocableFunctionPtr("execute"));
				errors += execute() != value;
			}
			return errors;
		});
	}
}

TEST_CASE("CPP backend compiles graphs without a unit id concurrently, 0.3 IR path") {
	runThreads([](int t) {
		int errors = 0;
		auto* backend = compiler::CompilationBackendRegistry::getInstance()->getBackend("cpp");
		for (int i = 0; i < PerThread; ++i) {
			const int32_t value = t * PerThread + i;
			auto ir = traceReturning(value);
			REQUIRE(ir->getId().empty());
			engine::Options options;
			compiler::DumpHandler dumpHandler(options, ir->getId());
			auto executable = backend->compile(ir, dumpHandler, options, nullptr);
			errors += executable->getInvocableMember<int32_t>("execute")() != value;
		}
		return errors;
	});
}

TEST_CASE("CPP backend compiles graphs without a unit id concurrently, NIR path") {
	runThreads([](int t) {
		int errors = 0;
		auto* backend = compiler::CompilationBackendRegistry::getInstance()->getBackend("cpp");
		for (int i = 0; i < PerThread; ++i) {
			const int32_t value = t * PerThread + i;
			auto parsed = nir::parse("nir 1.0\nfunc export @execute : () -> i32 {\n^b0:\n  ret " +
			                         std::to_string(value) + ":i32\n}\n");
			REQUIRE(parsed.ok());
			// the graph of the NIR front end carries the module and no IR
			auto ir = std::make_shared<compiler::ir::IRGraph>(compiler::CompilationUnitID {});
			ir->setNirModule(std::make_shared<const nir::Module>(std::move(*parsed.module)));
			engine::Options options;
			compiler::DumpHandler dumpHandler(options, ir->getId());
			auto executable = backend->compile(ir, dumpHandler, options, nullptr);
			errors += executable->getInvocableMember<int32_t>("execute")() != value;
		}
		return errors;
	});
}

#if defined(__unix__) || defined(__APPLE__)
// Processes share the temp directory too, and a pid is not unique across containers that share it.
TEST_CASE("CPP compiler names the files of concurrent processes uniquely") {
	std::vector<pid_t> children;
	for (int p = 0; p < 4; ++p) {
		const pid_t child = ::fork();
		REQUIRE(child >= 0);
		if (child == 0) {
			int errors = 0;
			try {
				for (int i = 0; i < PerThread; ++i) {
					const int value = p * PerThread + i;
					auto library = compiler::cpp::CPPCompiler::create()->compile("", sourceReturning(value));
					auto* execute = reinterpret_cast<int (*)()>(library->getInvocableFunctionPtr("execute"));
					errors += execute() != value;
				}
			} catch (...) {
				errors = 1;
			}
			::_exit(errors == 0 ? 0 : 1);
		}
		children.push_back(child);
	}
	for (const auto child : children) {
		int status = 0;
		REQUIRE(::waitpid(child, &status, 0) == child);
		REQUIRE(WIFEXITED(status));
		REQUIRE(WEXITSTATUS(status) == 0);
	}
}
#endif

} // namespace nautilus::engine
