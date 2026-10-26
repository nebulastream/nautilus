// The text form of extension types, operations, `extension` lines and intrinsic callees (13-nir-spec.md §9; M1.3a).
// The registry is process-wide state, so these tests live in this binary.
#include "nautilus/extensions/ExtensionRegistry.hpp"
#include "nautilus/nir/Parser.hpp"
#include "nautilus/nir/Printer.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::ext {
struct ExtensionRegistryTestAccess {
	static void reset();
};
} // namespace nautilus::ext

using namespace nautilus;
using namespace nautilus::ext;
using namespace nautilus::nir;

namespace {

void intrinsicFn() {
}

/// test.vec<f32,4> (16 bytes), test.add (two values, result of operand 0), test.splat (value and imm, i64),
/// test.shuffle (a list: prefix [value, imm], group [value, type]), the intrinsic test.fn (i64) -> i64.
struct Fixture {
	Fixture() {
		ExtensionRegistryTestAccess::reset();
		auto& r = ExtensionRegistry::instance();
		TypeInfo t;
		t.name = "test.vec<f32,4>";
		t.size = 16;
		t.align = 16;
		t.version = 2;
		r.registerType(t);

		ExtOpInfo o;
		o.hasExpansion = true;
		o.name = "test.add";
		o.shape.a = o.shape.b = FieldKind::Value;
		o.sig.operands = 2;
		o.sig.resultFromOperand = 0;
		r.registerOp(o);

		o = ExtOpInfo {};
		o.hasExpansion = true;
		o.name = "test.splat";
		o.shape.a = FieldKind::Value;
		o.shape.b = FieldKind::Imm;
		o.sig.operands = 1;
		o.sig.result = TypeId::I64;
		r.registerOp(o);

		o = ExtOpInfo {};
		o.hasExpansion = true;
		o.name = "test.shuffle";
		o.shape.c = FieldKind::List;
		o.shape.prefix = {WordKind::Value, WordKind::Imm};
		o.shape.group = {WordKind::Value, WordKind::Type};
		o.sig.operands = 2;
		o.sig.result = TypeId::I64;
		r.registerOp(o);

		IntrinsicInfo i;
		i.name = "test.fn";
		i.signature = {makeTypeWord(TypeId::I64), makeTypeWord(TypeId::I64)};
		i.attrs = 1 << 4; // noUnwind
		i.fallback = reinterpret_cast<void*>(&intrinsicFn);
		r.registerIntrinsic(i);
	}
};

std::string parseError(const std::string& text) {
	auto r = parse(text);
	return r.error ? r.error->str() : std::string();
}

const std::string vec = "!test.vec<f32,4>";
const std::string zeros = "x\"00000000000000000000000000000000\":" + vec;

} // namespace

TEST_CASE("extension types, operations and intrinsics round-trip through the text form", "[text][extension]") {
	Fixture fixture;
	std::string text = "nir 1.0\n"
	                   "module \"ext\"\n"
	                   "extension \"test.vec<f32,4>@2\"\n"
	                   "extension \"test.add@1\"\n"
	                   "extension \"test.fn@1\"\n"
	                   "callee #c0 = intrinsic test.fn : (i64) -> i64 attrs(noUnwind)\n"
	                   "\n"
	                   "func export @f : (i64) -> i64 {\n"
	                   "^b0(%0: i64):\n"
	                   "  %1 = test.add " +
	                   zeros + ", " + zeros + " : " + vec +
	                   "\n"
	                   "  %2 = test.splat %0, 3 : i64\n"
	                   "  %3 = test.shuffle [%0, 1, %2, i64] : i64\n"
	                   "  %4 = call #c0(%3)\n"
	                   "  ret %4\n"
	                   "}\n";
	auto r = parse(text);
	INFO((r.error ? r.error->str() : std::string()));
	REQUIRE(r.ok());
	CHECK(r.module->extensions.size() == 3);
	CHECK(print(*r.module) == text);
}

TEST_CASE("an extension type is a single token and a constant of one is bytes of its size", "[text][extension]") {
	Fixture fixture;
	auto r = parse("nir 1.0\nfunc export @f : () -> i64 {\n^b0:\n  %0 = test.add " + zeros + ", " + zeros + " : " +
	               vec + "\n  ret 0:i64\n}\n");
	REQUIRE(r.ok());
	auto& c = r.module->consts.at(0);
	CHECK(c.type == static_cast<uint8_t>(*ExtensionRegistry::instance().findType("test.vec<f32,4>")));
	CHECK(r.module->consts.bytes().size() == 16);
	// the wrong size is a literal error
	CHECK(parseError("nir 1.0\nfunc export @f : () -> i64 {\n^b0:\n  %0 = test.add x\"00\":" + vec + ", " + zeros +
	                 " : " + vec + "\n  ret 0:i64\n}\n")
	          .find("NIR-P006") != std::string::npos);
}

TEST_CASE("an extension line must name a registered extension in an accepted version", "[text][extension]") {
	Fixture fixture;
	CHECK(parseError("nir 1.0\nextension \"test.unknown@1\"\n").find("NIR-P005") != std::string::npos);
	CHECK(parseError("nir 1.0\nextension \"test.add\"\n").find("NIR-P005") != std::string::npos);
	// test.add is at version 1; a text that expects a newer one is rejected
	CHECK(parseError("nir 1.0\nextension \"test.add@2\"\n").find("NIR-P005") != std::string::npos);
	CHECK(parseError("nir 1.0\nextension \"test.add@1\"\n").empty());
	CHECK(parseError("nir 1.0\ncallee #c0 = intrinsic test.nothing : () -> void\n").find("NIR-P005") !=
	      std::string::npos);
}

TEST_CASE("an intrinsic callee without an extension line gets its record", "[text][extension]") {
	Fixture fixture;
	auto r = parse("nir 1.0\ncallee #c0 = intrinsic test.fn : (i64) -> i64 attrs(noUnwind)\n");
	REQUIRE(r.ok());
	REQUIRE(r.module->extensions.size() == 1);
	CHECK(r.module->name(r.module->extensions[0].nameId) == "test.fn@1");
	// printing adds the line, and the result is stable
	auto printed = print(*r.module);
	CHECK(printed.find("extension \"test.fn@1\"") != std::string::npos);
	auto again = parse(printed);
	REQUIRE(again.ok());
	CHECK(print(*again.module) == printed);
}
