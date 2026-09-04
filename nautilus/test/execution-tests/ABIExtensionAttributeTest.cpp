#include "nautilus/config.hpp"

#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)

#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace nautilus::engine {

namespace {

// `llvm-diff` ignores parameter and result attributes, so the reference-IR
// suite in test/llvm-ir-test cannot observe an ABI extension attribute
// appearing or disappearing: two modules differing only by `zeroext i1` vs
// `i1` compare equal there. These tests therefore assert on the text of the
// generated `define` line directly -- that is the only thing standing between
// a dropped extension attribute and a silent ABI regression.

// Whether the host ABI has the caller extend narrow integer arguments, and so
// whether the entry function is entitled to trust that it did. AAPCS64 leaves
// the bits above a narrow argument unspecified; Darwin AArch64 and x86-64 SysV
// require the caller to extend. Mirrors callerExtendsNarrowArguments() in
// MLIRLoweringProvider.cpp -- annotating parameters on AAPCS64 makes the callee
// read register bits nothing ever set, which is a wrong-answer bug and not a
// missed optimization.
#if defined(__aarch64__) && !defined(__APPLE__)
constexpr bool kCallerExtendsNarrowArguments = false;
#else
constexpr bool kCallerExtendsNarrowArguments = true;
#endif

val<bool> abiBoolIdentity(val<bool> x) {
	return x;
}

val<int8_t> abiInt8Identity(val<int8_t> x) {
	return x;
}

val<uint8_t> abiUInt8Identity(val<uint8_t> x) {
	return x;
}

val<int16_t> abiInt16Identity(val<int16_t> x) {
	return x;
}

val<uint16_t> abiUInt16Identity(val<uint16_t> x) {
	return x;
}

val<int32_t> abiInt32Identity(val<int32_t> x) {
	return x;
}

val<int32_t> abiMixedNarrowArgs(val<bool> flag, val<int8_t> a, val<uint16_t> b, val<int32_t> c) {
	val<int32_t> widened = c + static_cast<val<int32_t>>(a) + static_cast<val<int32_t>>(b);
	return flag ? widened : val<int32_t>(0);
}

std::string readWholeFile(std::string_view path) {
	std::ifstream in(std::string(path).c_str());
	std::ostringstream buffer;
	buffer << in.rdbuf();
	return buffer.str();
}

/// Compiles @p func with the MLIR backend and returns the `define` line of the
/// generated entry function from the `after_llvm_generation` dump.
///
/// The dump file is located through the executable's own record of what it
/// wrote rather than by scanning the shared dump root, so a concurrently
/// running test cannot hand this one another test's module.
template <typename Func>
std::string entryDefineLine(Func func) {
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("dump.after_llvm_generation", true);

	NautilusEngine engine(options);
	auto compiled = engine.registerFunction(func);

	const auto generatedFile = compiled.getExecutable()->getGeneratedFile("after_llvm_generation");
	REQUIRE(!generatedFile.empty());

	const auto contents = readWholeFile(generatedFile);

	// Drop this compilation's dump directory again now that it has been read,
	// as the llvm-ir test helper does. Several debug-info tests locate their
	// own output by diffing the shared dump root before and after compiling,
	// so every directory left behind here is one more candidate they have to
	// sift through.
	const auto dumpDir = std::filesystem::path(std::string(generatedFile)).parent_path();
	if (dumpDir.parent_path() == std::filesystem::temp_directory_path() / "dump") {
		std::error_code ec;
		std::filesystem::remove_all(dumpDir, ec);
	}

	std::istringstream iss(contents);
	std::string line;
	while (std::getline(iss, line)) {
		// `llvm.emit_c_interface` also emits `_mlir_ciface_execute` plus the
		// packed `_mlir_*` wrappers for the same function. The bare symbol is
		// the one MLIRExecutable::getInvocableFunctionPtr resolves and
		// Executable.hpp calls through a function pointer typed with the
		// traced signature, so it is the one whose signature has to state the
		// ABI contract.
		if (line.rfind("define", 0) == 0 && line.find(" @execute(") != std::string::npos) {
			return line;
		}
	}
	return {};
}

/// Splits the parameter list out of a `define` line into one string per
/// parameter, each of the shape `<type> [attrs...] %<n>`.
std::vector<std::string> parameterList(const std::string& define) {
	const auto open = define.find('(');
	const auto close = define.find(')', open);
	REQUIRE(open != std::string::npos);
	REQUIRE(close != std::string::npos);

	const auto params = define.substr(open + 1, close - open - 1);
	std::vector<std::string> parts;
	std::istringstream paramStream(params);
	std::string part;
	while (std::getline(paramStream, part, ',')) {
		parts.push_back(part);
	}
	return parts;
}

bool hasExtensionAttr(const std::string& parameter) {
	return parameter.find("signext") != std::string::npos || parameter.find("zeroext") != std::string::npos;
}

/// Asserts the single parameter and the result of an identity function both
/// carry @p attr. LLVM prints a parameter's attributes *after* its type
/// (`i8 signext %0`, sometimes with an optimizer-inserted `returned` in
/// between) but a result's attribute *before* it (`define signext i8 @...`),
/// so the two are matched differently on purpose.
void checkIdentitySignature(const std::string& define, std::string_view type, std::string_view attr) {
	INFO("define line: " << define);

	const auto params = parameterList(define);
	REQUIRE(params.size() == 1);
	CHECK(params[0].find(type) != std::string::npos);
	if (kCallerExtendsNarrowArguments) {
		CHECK(params[0].find(attr) != std::string::npos);
	} else {
		CHECK(params[0].find(attr) == std::string::npos);
	}

	// The result side is unconditional: an extension attribute there is a
	// promise the callee makes and keeps, so a caller that re-extends anyway is
	// merely redundant. That is why #436's fix is safe on every ABI.
	CHECK(define.rfind("define " + std::string(attr) + " " + std::string(type) + " ", 0) == 0);
}

} // namespace

TEST_CASE("ABI: entry function bool parameter and result carry zeroext") {
	const auto define = entryDefineLine(abiBoolIdentity);
	REQUIRE_FALSE(define.empty());
	checkIdentitySignature(define, "i1", "zeroext");
}

TEST_CASE("ABI: entry function int8_t parameter and result carry signext") {
	const auto define = entryDefineLine(abiInt8Identity);
	REQUIRE_FALSE(define.empty());
	checkIdentitySignature(define, "i8", "signext");
}

TEST_CASE("ABI: entry function uint8_t parameter and result carry zeroext") {
	const auto define = entryDefineLine(abiUInt8Identity);
	REQUIRE_FALSE(define.empty());
	checkIdentitySignature(define, "i8", "zeroext");
}

TEST_CASE("ABI: entry function int16_t parameter and result carry signext") {
	const auto define = entryDefineLine(abiInt16Identity);
	REQUIRE_FALSE(define.empty());
	checkIdentitySignature(define, "i16", "signext");
}

TEST_CASE("ABI: entry function uint16_t parameter and result carry zeroext") {
	const auto define = entryDefineLine(abiUInt16Identity);
	REQUIRE_FALSE(define.empty());
	checkIdentitySignature(define, "i16", "zeroext");
}

TEST_CASE("ABI: entry function 32-bit parameter carries no extension attribute") {
	// A 32-bit argument already occupies a full argument register on every
	// target Nautilus builds for, so it needs no extension attribute; the
	// classification must stop at 16 bits rather than annotating every
	// integer stamp.
	const auto define = entryDefineLine(abiInt32Identity);
	REQUIRE_FALSE(define.empty());
	INFO("define line: " << define);

	const auto params = parameterList(define);
	REQUIRE(params.size() == 1);
	CHECK(params[0].find("i32") != std::string::npos);
	CHECK_FALSE(hasExtensionAttr(params[0]));
}

TEST_CASE("ABI: mixed-width parameters are annotated positionally") {
	// The attributes are keyed by argument index, so a signature mixing narrow
	// and wide stamps is where an off-by-one would surface: `flag` must be
	// zeroext, `a` signext, `b` zeroext, and `c` unannotated.
	const auto define = entryDefineLine(abiMixedNarrowArgs);
	REQUIRE_FALSE(define.empty());
	INFO("define line: " << define);

	const auto params = parameterList(define);
	REQUIRE(params.size() == 4);

	CHECK(params[0].find("i1") != std::string::npos);
	CHECK(params[1].find("i8") != std::string::npos);
	CHECK(params[2].find("i16") != std::string::npos);
	CHECK(params[3].find("i32") != std::string::npos);

	if (kCallerExtendsNarrowArguments) {
		CHECK(params[0].find("zeroext") != std::string::npos);
		CHECK(params[1].find("signext") != std::string::npos);
		CHECK(params[2].find("zeroext") != std::string::npos);
	} else {
		CHECK_FALSE(hasExtensionAttr(params[0]));
		CHECK_FALSE(hasExtensionAttr(params[1]));
		CHECK_FALSE(hasExtensionAttr(params[2]));
	}

	// A 32-bit argument is unannotated on every ABI.
	CHECK_FALSE(hasExtensionAttr(params[3]));
}

TEST_CASE("ABI: narrow arguments round-trip through the native call boundary") {
	// The attributes above move the entry function from "re-extends every
	// narrow argument defensively" to "trusts the caller to have extended it".
	// That is only sound if the caller on the other side of
	// Executable.hpp's function pointer really does extend, so exercise the
	// boundary with the values whose high bits differ between a zero- and a
	// sign-extended register.
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	NautilusEngine engine(options);

	auto boolFn = engine.registerFunction(abiBoolIdentity);
	CHECK(boolFn(true) == true);
	CHECK(boolFn(false) == false);

	auto int8Fn = engine.registerFunction(abiInt8Identity);
	CHECK(int8Fn(int8_t(-1)) == int8_t(-1));
	CHECK(int8Fn(int8_t(-128)) == int8_t(-128));
	CHECK(int8Fn(int8_t(127)) == int8_t(127));

	auto uint8Fn = engine.registerFunction(abiUInt8Identity);
	CHECK(uint8Fn(uint8_t(255)) == uint8_t(255));
	CHECK(uint8Fn(uint8_t(0)) == uint8_t(0));

	auto int16Fn = engine.registerFunction(abiInt16Identity);
	CHECK(int16Fn(int16_t(-1)) == int16_t(-1));
	CHECK(int16Fn(int16_t(-32768)) == int16_t(-32768));

	auto uint16Fn = engine.registerFunction(abiUInt16Identity);
	CHECK(uint16Fn(uint16_t(65535)) == uint16_t(65535));

	auto mixedFn = engine.registerFunction(abiMixedNarrowArgs);
	CHECK(mixedFn(true, int8_t(-1), uint16_t(65535), 1) == 1 + (-1) + 65535);
	CHECK(mixedFn(false, int8_t(-1), uint16_t(65535), 1) == 0);
}

} // namespace nautilus::engine

#endif
