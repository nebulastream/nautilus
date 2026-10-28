// nautilus-nir-run: compile SSA-level NIR with a NIR backend and run the calls the file asks for.
//
//   nautilus-nir-run [--backend=NAME] [--option=KEY=VALUE]... [--dump] file.nir
//
// The directives are comment lines of the text form:
//
//   ; run @name(arg, ...) = result          call an exported function and compare the result
//   ; run @name(arg, ...) throws KIND        the call throws: ArithmeticException, runtime_error or any
//   ; log 1, 2, 3                           the calls of rt_log made by the preceding run, in order
//
// Arguments and results: integers (decimal or 0x..., compared at the width of the type), floats, `true`/`false`,
// `null`, `buf` (a pointer to 256 zeroed bytes, 16-aligned, fresh for every run) and `nan`.
//
// Pointers of the form `symbol "name"` resolve against the host functions below; bindings are zeroed buffers of their
// declared size. Exit status: 0 all directives hold, 1 a directive failed or the module is invalid, 2 usage or I/O
// error, 77 a backend mode that is not available in this build was required (ctest reads it as a skip).
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Executable.hpp"
#include "nautilus/common/ExceptionTransport.hpp"
#include "nautilus/compiler/backends/nir/NirBackend.hpp"
#include "nautilus/conversion/Conversion.hpp"
#include "nautilus/exceptions/ArithmeticException.hpp"
#include "nautilus/nir/Module.hpp"
#include "nautilus/nir/Parser.hpp"
#include "nautilus/nir/Printer.hpp"
#include "nautilus/nir/Verifier.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>
#ifdef NIR_RUN_DYNCALL
#include <dyncall.h>
#endif

using namespace nautilus;
using namespace nautilus::compiler;

namespace {

std::vector<int64_t> g_log;

// ---- host functions that the test modules call ---------------------------------------------------------------------

int64_t rtAdd(int64_t a, int64_t b) {
	return a + b;
}
int32_t rtAdd32(int32_t a, int32_t b) {
	return a + b;
}
double rtMulF(double a, double b) {
	return a * b;
}
void rtLog(int64_t v) {
	g_log.push_back(v);
}
void rtThrow(int64_t code) {
	if (code == 0) {
		return;
	}
	g_log.push_back(-code);
	throw std::runtime_error("rt_throw " + std::to_string(code));
}
/// a destructor: logs the 8 bytes its object holds
void rtDestroy(void* object) {
	int64_t v;
	std::memcpy(&v, object, sizeof(v));
	g_log.push_back(v);
}
NAUTILUS_NO_SANITIZE_FUNCTION int64_t rtCallback(int64_t (*f)(int64_t), int64_t v) {
	return f(v);
}
int64_t rtDouble(int64_t v) {
	return 2 * v;
}
uint8_t rtNarrow(uint8_t v) {
	return static_cast<uint8_t>(v + 1);
}

void* resolveHostSymbol(std::string_view name) {
	static const std::pair<std::string_view, void*> table[] = {
	    {"rt_add", reinterpret_cast<void*>(&rtAdd)},
	    {"rt_add32", reinterpret_cast<void*>(&rtAdd32)},
	    {"rt_mulf", reinterpret_cast<void*>(&rtMulF)},
	    {"rt_log", reinterpret_cast<void*>(&rtLog)},
	    {"rt_log$thunk", reinterpret_cast<void*>(&compiler::captureThrowingCall<void, int64_t>)},
	    {"rt_throw", reinterpret_cast<void*>(&rtThrow)},
	    {"rt_throw$thunk", reinterpret_cast<void*>(&compiler::captureThrowingCall<void, int64_t>)},
	    {"rt_add$thunk", reinterpret_cast<void*>(&compiler::captureThrowingCall<int64_t, int64_t, int64_t>)},
	    {"rt_destroy", reinterpret_cast<void*>(&rtDestroy)},
	    {"rt_callback", reinterpret_cast<void*>(&rtCallback)},
	    {"rt_double", reinterpret_cast<void*>(&rtDouble)},
	    {"rt_double$thunk", reinterpret_cast<void*>(&compiler::captureThrowingCall<int64_t, int64_t>)},
	    {"rt_narrow", reinterpret_cast<void*>(&rtNarrow)},
	};
	for (const auto& [n, address] : table) {
		if (n == name) {
			return address;
		}
	}
	return nullptr;
}

// ---- directives ---------------------------------------------------------------------------------------------------

struct Directive {
	int line = 0;
	std::string function;
	std::vector<std::string> args;
	bool throws = false;
	std::string kind; // throws: the kind of exception
	std::string expected;
	bool hasLog = false;
	std::vector<int64_t> log;
};

std::string trim(std::string s) {
	const auto b = s.find_first_not_of(" \t\r");
	if (b == std::string::npos) {
		return "";
	}
	return s.substr(b, s.find_last_not_of(" \t\r") - b + 1);
}

std::vector<std::string> split(const std::string& s, char sep) {
	std::vector<std::string> parts;
	std::string current;
	for (char c : s) {
		if (c == sep) {
			parts.push_back(trim(current));
			current.clear();
		} else {
			current += c;
		}
	}
	if (!trim(current).empty() || !parts.empty()) {
		parts.push_back(trim(current));
	}
	return parts;
}

std::vector<Directive> readDirectives(const std::string& text) {
	std::vector<Directive> directives;
	std::istringstream in(text);
	std::string line;
	int number = 0;
	while (std::getline(in, line)) {
		++number;
		auto t = trim(line);
		if (t.rfind("; run ", 0) == 0) {
			Directive d;
			d.line = number;
			auto rest = trim(t.substr(6));
			const auto open = rest.find('(');
			const auto close = rest.find(')');
			if (rest.empty() || rest[0] != '@' || open == std::string::npos || close == std::string::npos) {
				throw std::runtime_error("line " + std::to_string(number) + ": malformed run directive");
			}
			d.function = rest.substr(1, open - 1);
			d.args = split(rest.substr(open + 1, close - open - 1), ',');
			auto tail = trim(rest.substr(close + 1));
			if (tail.rfind("throws", 0) == 0) {
				d.throws = true;
				d.kind = trim(tail.substr(6));
				if (d.kind.empty()) {
					d.kind = "any";
				}
			} else if (tail.rfind("=", 0) == 0) {
				d.expected = trim(tail.substr(1));
			} else if (!tail.empty()) {
				throw std::runtime_error("line " + std::to_string(number) + ": expected '= result' or 'throws'");
			}
			directives.push_back(std::move(d));
		} else if (t.rfind("; log", 0) == 0) {
			if (directives.empty()) {
				throw std::runtime_error("line " + std::to_string(number) + ": log without a run");
			}
			auto& d = directives.back();
			d.hasLog = true;
			for (const auto& v : split(trim(t.substr(5)), ',')) {
				d.log.push_back(std::strtoll(v.c_str(), nullptr, 0));
			}
		}
	}
	return directives;
}

// ---- values -------------------------------------------------------------------------------------------------------

uint64_t widthMask(unsigned bits) {
	return bits >= 64 ? ~uint64_t {0} : (uint64_t {1} << bits) - 1;
}

/// The raw 64-bit slot of a literal for a parameter or result of type `type`.
uint64_t parseValue(const std::string& text, nir::TypeId type, std::vector<std::vector<std::byte>>& buffers) {
	switch (type) {
	case nir::TypeId::Bool:
		if (text == "true" || text == "false") {
			return text == "true";
		}
		return std::strtoull(text.c_str(), nullptr, 0) != 0;
	case nir::TypeId::F32: {
		float f = text == "nan" ? std::nanf("") : std::strtof(text.c_str(), nullptr);
		uint32_t bits;
		std::memcpy(&bits, &f, 4);
		return bits;
	}
	case nir::TypeId::F64: {
		double d = text == "nan" ? std::nan("") : std::strtod(text.c_str(), nullptr);
		uint64_t bits;
		std::memcpy(&bits, &d, 8);
		return bits;
	}
	case nir::TypeId::Ptr: {
		if (text == "null") {
			return 0;
		}
		if (text == "buf") {
			buffers.emplace_back(272);
			auto address = reinterpret_cast<uintptr_t>(buffers.back().data());
			return (address + 15) & ~uintptr_t {15};
		}
		return std::strtoull(text.c_str(), nullptr, 0);
	}
	case nir::TypeId::I8:
	case nir::TypeId::I16:
	case nir::TypeId::I32:
	case nir::TypeId::I64: {
		const unsigned bits = nir::sizeOf(type) * 8;
		uint64_t value = text[0] == '-' ? static_cast<uint64_t>(std::strtoll(text.c_str(), nullptr, 0))
		                                : std::strtoull(text.c_str(), nullptr, 0);
		return value & widthMask(bits);
	}
	default:
		throw std::runtime_error("an argument or result of this type cannot be written in a directive");
	}
}

bool sameValue(uint64_t a, uint64_t b, nir::TypeId type) {
	if (nir::isFloat(type)) {
		if (type == nir::TypeId::F32) {
			float x, y;
			const auto xa = static_cast<uint32_t>(a), xb = static_cast<uint32_t>(b);
			std::memcpy(&x, &xa, 4);
			std::memcpy(&y, &xb, 4);
			return (std::isnan(x) && std::isnan(y)) || a == b;
		}
		double x, y;
		std::memcpy(&x, &a, 8);
		std::memcpy(&y, &b, 8);
		return (std::isnan(x) && std::isnan(y)) || a == b;
	}
	const uint64_t mask = type == nir::TypeId::Bool ? 1 : widthMask(nir::sizeOf(type) * 8);
	return (a & mask) == (b & mask);
}

std::string show(uint64_t v, nir::TypeId type) {
	if (type == nir::TypeId::F32) {
		float f;
		const auto bits = static_cast<uint32_t>(v);
		std::memcpy(&f, &bits, 4);
		return std::to_string(f);
	}
	if (type == nir::TypeId::F64) {
		double d;
		std::memcpy(&d, &v, 8);
		return std::to_string(d);
	}
	const unsigned bits = nir::sizeOf(type) * 8;
	if (nir::isInteger(type) && bits < 64 && (v >> (bits - 1)) & 1) {
		return std::to_string(static_cast<int64_t>(v | ~widthMask(bits)));
	}
	return std::to_string(static_cast<int64_t>(v));
}

#ifdef NIR_RUN_DYNCALL
/// Calls the machine code of a native executable (`cpp`, ...) with the raw slots of the arguments, as the C ABI of the
/// signature wants them: a narrow integer extended the way its type word says, a float or double bit-exact.
uint64_t callNative(void* target, const nir::Module& module, const nir::SignatureRecord& signature,
                    const std::vector<uint64_t>& args) {
	DCCallVM* vm = dcNewCallVM(4096);
	dcMode(vm, DC_CALL_C_DEFAULT);
	dcReset(vm);
	for (uint32_t i = 0; i < signature.paramCount; ++i) {
		const auto word = module.paramType(signature, i);
		const auto bits = args[i];
		const bool isUnsigned = nir::extKindOf(word) == nir::ExtKind::Zext;
		switch (nir::typeOf(word)) {
		case nir::TypeId::Bool:
			dcArgBool(vm, bits != 0);
			break;
		case nir::TypeId::I8:
			dcArgInt(vm, isUnsigned ? static_cast<DCint>(static_cast<uint8_t>(bits))
			                        : static_cast<DCint>(static_cast<int8_t>(bits)));
			break;
		case nir::TypeId::I16:
			dcArgInt(vm, isUnsigned ? static_cast<DCint>(static_cast<uint16_t>(bits))
			                        : static_cast<DCint>(static_cast<int16_t>(bits)));
			break;
		case nir::TypeId::I32:
			dcArgInt(vm, static_cast<DCint>(static_cast<uint32_t>(bits)));
			break;
		case nir::TypeId::I64:
			dcArgLongLong(vm, static_cast<DClonglong>(bits));
			break;
		case nir::TypeId::F32: {
			const auto raw = static_cast<uint32_t>(bits);
			float value;
			std::memcpy(&value, &raw, sizeof(value));
			dcArgFloat(vm, value);
			break;
		}
		case nir::TypeId::F64: {
			double value;
			std::memcpy(&value, &bits, sizeof(value));
			dcArgDouble(vm, value);
			break;
		}
		case nir::TypeId::Ptr:
			dcArgPointer(vm, reinterpret_cast<void*>(bits));
			break;
		default:
			dcFree(vm);
			throw std::runtime_error("an argument type that cannot be passed natively");
		}
	}
	uint64_t result = 0;
	switch (nir::typeOf(signature.result)) {
	case nir::TypeId::Void:
		dcCallVoid(vm, target);
		break;
	case nir::TypeId::Bool:
		result = (dcCallBool(vm, target) & 0xFF) != 0 ? 1 : 0;
		break;
	case nir::TypeId::I8:
		result = static_cast<uint8_t>(dcCallChar(vm, target));
		break;
	case nir::TypeId::I16:
		result = static_cast<uint16_t>(dcCallShort(vm, target));
		break;
	case nir::TypeId::I32:
		result = static_cast<uint32_t>(dcCallInt(vm, target));
		break;
	case nir::TypeId::I64:
		result = static_cast<uint64_t>(dcCallLongLong(vm, target));
		break;
	case nir::TypeId::F32: {
		const float value = dcCallFloat(vm, target);
		uint32_t raw;
		std::memcpy(&raw, &value, sizeof(raw));
		result = raw;
		break;
	}
	case nir::TypeId::F64: {
		const double value = dcCallDouble(vm, target);
		std::memcpy(&result, &value, sizeof(result));
		break;
	}
	case nir::TypeId::Ptr:
		result = reinterpret_cast<uint64_t>(dcCallPointer(vm, target));
		break;
	default:
		dcFree(vm);
		throw std::runtime_error("a result type that cannot be returned natively");
	}
	dcFree(vm);
	return result;
}
#endif

} // namespace

int main(int argc, char** argv) {
	std::string backendName = "tbc", path;
	engine::Options options;
	bool dump = false;
	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];
		if (arg.rfind("--backend=", 0) == 0) {
			backendName = arg.substr(10);
		} else if (arg.rfind("--option=", 0) == 0) {
			auto kv = arg.substr(9);
			const auto eq = kv.find('=');
			if (eq == std::string::npos) {
				std::cerr << "--option expects KEY=VALUE\n";
				return 2;
			}
			auto key = kv.substr(0, eq), value = kv.substr(eq + 1);
			if (value == "true" || value == "false") {
				options.setOption(key, value == "true");
			} else if (!value.empty() && value.find_first_not_of("0123456789") == std::string::npos) {
				options.setOption(key, std::stoi(value));
			} else {
				options.setOption(key, value);
			}
		} else if (arg == "--dump") {
			dump = true;
		} else if (arg == "-h" || arg == "--help") {
			std::cout << "usage: nautilus-nir-run [--backend=NAME] [--option=KEY=VALUE]... [--dump] file.nir\n";
			return 0;
		} else if (arg.size() > 1 && arg[0] == '-') {
			std::cerr << "unknown option " << arg << "\n";
			return 2;
		} else {
			path = arg;
		}
	}
	if (path.empty()) {
		std::cerr << "no input file\n";
		return 2;
	}
	std::ifstream in(path, std::ios::binary);
	if (!in) {
		std::cerr << "cannot read " << path << "\n";
		return 2;
	}
	std::stringstream buffer;
	buffer << in.rdbuf();
	const std::string text = buffer.str();

	auto parsed = nir::parse(text);
	if (!parsed.ok()) {
		if (parsed.error) {
			std::cerr << path << ": " << parsed.error->str() << "\n";
		}
		for (const auto& d : parsed.verifierDiagnostics) {
			std::cerr << path << ": " << d.str() << "\n";
		}
		return 1;
	}
	auto module = std::make_shared<nir::Module>(std::move(*parsed.module));
	// pre-SSA input is converted first, so the conversion can be run on text
	const bool preSSA = std::any_of(module->functions.begin(), module->functions.end(),
	                                [](const auto& f) { return f.level == static_cast<uint8_t>(nir::Level::PreSSA); });
	if (preSSA) {
		try {
			*module = conversion::convert(*module);
		} catch (const std::exception& e) {
			std::cerr << path << ": conversion failed: " << e.what() << "\n";
			return 1;
		}
		const auto diagnostics = nir::verify(*module);
		for (const auto& d : diagnostics) {
			std::cerr << path << ": after the conversion: " << d.str() << "\n";
		}
		if (!diagnostics.empty()) {
			if (dump) {
				std::cout << nir::print(*module);
			}
			return 1;
		}
	}
	if (dump) {
		std::cout << nir::print(*module);
	}

	std::vector<Directive> directives;
	try {
		directives = readDirectives(text);
	} catch (const std::exception& e) {
		std::cerr << path << ": " << e.what() << "\n";
		return 2;
	}

	// tbc.mode=jit fails a compile that cannot stitch; here that is a skip, so ask for `auto` and look at what happened
	const bool requireJit = options.getOptionOrDefault<std::string>("tbc.mode", "interp") == "jit";
	if (requireJit) {
		options.setOption("tbc.mode", std::string("auto"));
	}
	const auto* backend = nirbackend::NirBackendRegistry::instance().find(backendName);
	if (backend == nullptr) {
		std::cerr << "no NIR backend \"" << backendName << "\" in this build\n";
		return 2;
	}

	// the bindings are zeroed buffers that live as long as the executable
	std::vector<std::vector<std::byte>> bindingStorage;
	nirbackend::HostEnvironment host;
	host.resolveSymbol = resolveHostSymbol;
	host.bindingAddress = [&](const nir::Module& m, uint32_t index) -> void* {
		bindingStorage.resize(std::max<size_t>(bindingStorage.size(), m.bindings.size()));
		auto& store = bindingStorage[index];
		const auto& b = m.bindings[index];
		store.assign(b.size + b.align, std::byte {0});
		auto address = reinterpret_cast<uintptr_t>(store.data());
		return reinterpret_cast<void*>((address + b.align - 1) & ~uintptr_t {b.align - 1});
	};

	compiler::CompilationUnitID unitId = "nir-run";
	compiler::DumpHandler dumpHandler(options, unitId);
	compiler::CompilationStatistics statistics;
	nirbackend::CompileContext context {dumpHandler, options, &statistics, nullptr, host};
	std::unique_ptr<Executable> executable;
	try {
		executable = backend->compile(module, context);
	} catch (const std::exception& e) {
		std::cerr << path << ": compilation failed: " << e.what() << "\n";
		return 1;
	}

	// a mode that the build cannot provide is a skip, not a pass on the wrong engine
	if (requireJit) {
		const auto* mode = statistics.find("tbc.mode");
		if (mode == nullptr || !std::holds_alternative<std::string>(*mode) || std::get<std::string>(*mode) != "jit") {
			std::cerr << "skipped: the copy-and-patch mode is not available in this build\n";
			return 77;
		}
	}

	int failures = 0;
	for (const auto& d : directives) {
		auto fail = [&](const std::string& why) {
			std::cerr << path << ":" << d.line << ": @" << d.function << ": " << why << "\n";
			++failures;
		};
		uint32_t functionIndex = ~0u;
		for (uint32_t f = 0; f < module->functions.size(); ++f) {
			if (module->name(module->functions[f].nameId) == d.function) {
				functionIndex = f;
			}
		}
		if (functionIndex == ~0u) {
			fail("no such function");
			continue;
		}
		const auto& signature = module->signatures[module->functions[functionIndex].signature];
		if (signature.paramCount != d.args.size()) {
			fail("expected " + std::to_string(signature.paramCount) + " arguments");
			continue;
		}
		std::vector<std::vector<std::byte>> buffers;
		std::vector<uint64_t> args;
		try {
			for (size_t i = 0; i < d.args.size(); ++i) {
				args.push_back(parseValue(d.args[i], nir::typeOf(module->paramType(signature, i)), buffers));
			}
		} catch (const std::exception& e) {
			fail(e.what());
			continue;
		}
		const auto resultType = nir::typeOf(signature.result);

		g_log.clear();
		uint64_t result = 0;
		std::string thrown;
		bool threwArithmetic = false;
		{
			auto invocable = executable->getGenericInvocable(d.function);
			ExceptionFrameScope scope(executable->getExceptionPropagationMode(d.function) ==
			                          ExceptionPropagationMode::CapturedHostRethrow);
			try {
				if (invocable != nullptr) {
					if (!invocable->invokeRaw(args.data(), args.size(), &result)) {
						fail("the backend has no raw invocation");
						continue;
					}
				} else if (executable->hasInvocableFunctionPtr()) {
					// a native executable: the machine code is called through its signature
#ifdef NIR_RUN_DYNCALL
					result = callNative(executable->getInvocableFunctionPtr(d.function), *module, signature, args);
#else
					fail("this build of nir-run cannot call a native executable (it needs dyncall)");
					continue;
#endif
				} else {
					fail("the backend offers neither a raw invocation nor a function pointer");
					continue;
				}
				if (scope.pending()) {
					std::rethrow_exception(scope.pending());
				}
			} catch (const ArithmeticException& e) {
				thrown = "ArithmeticException";
				threwArithmetic = true;
			} catch (const std::runtime_error& e) {
				thrown = "runtime_error";
			} catch (const std::exception& e) {
				thrown = "exception";
			}
		}
		if (d.throws) {
			if (thrown.empty()) {
				fail("expected an exception, got " + show(result, resultType));
			} else if (d.kind != "any" && d.kind != thrown && !(d.kind == "exception")) {
				fail("expected " + d.kind + ", got " + thrown);
			}
			(void) threwArithmetic;
		} else if (!thrown.empty()) {
			fail("unexpected " + thrown);
		} else if (resultType != nir::TypeId::Void) {
			std::vector<std::vector<std::byte>> none;
			const auto expected = parseValue(d.expected, resultType, none);
			if (!sameValue(result, expected, resultType)) {
				fail("expected " + d.expected + ", got " + show(result, resultType));
			}
		}
		if (d.hasLog && d.log != g_log) {
			std::string got;
			for (auto v : g_log) {
				got += (got.empty() ? "" : ", ") + std::to_string(v);
			}
			fail("expected the log [" +
			     [&] {
				     std::string e;
				     for (auto v : d.log) {
					     e += (e.empty() ? "" : ", ") + std::to_string(v);
				     }
				     return e;
			     }() +
			     "], got [" + got + "]");
		}
	}
	if (failures == 0) {
		std::cout << path << ": " << directives.size() << " calls hold\n";
	}
	return failures == 0 ? 0 : 1;
}
