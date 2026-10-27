// The text form of extension types, operations, `extension` lines and intrinsic callees (13-nir-spec.md §9; M1.3a).
// The registry is process-wide state, so these tests live in this binary.
#include "nautilus/extensions/ExtensionRegistry.hpp"
#include "nautilus/nir/Hash.hpp"
#include "nautilus/nir/Image.hpp"
#include "nautilus/nir/ImageLoader.hpp"
#include "nautilus/nir/Parser.hpp"
#include "nautilus/nir/Printer.hpp"
#include <algorithm>
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
	/// `reversed` registers the operations in the other order, so that every process opcode differs.
	explicit Fixture(bool reversed = false, uint32_t splatVersion = 1) {
		ExtensionRegistryTestAccess::reset();
		auto& r = ExtensionRegistry::instance();
		TypeInfo t;
		t.name = "test.vec<f32,4>";
		t.size = 16;
		t.align = 16;
		t.version = 2;
		r.registerType(t);

		std::vector<ExtOpInfo> ops(3);
		ops[0].name = "test.add";
		ops[0].shape.a = ops[0].shape.b = FieldKind::Value;
		ops[0].sig.operands = 2;
		ops[0].sig.resultFromOperand = 0;
		ops[1].name = "test.splat";
		ops[1].version = ops[1].minCompatibleVersion = splatVersion;
		ops[1].shape.a = FieldKind::Value;
		ops[1].shape.b = FieldKind::Imm;
		ops[1].sig.operands = 1;
		ops[1].sig.result = TypeId::I64;
		ops[2].name = "test.shuffle";
		ops[2].shape.c = FieldKind::List;
		ops[2].shape.prefix = {WordKind::Value, WordKind::Imm};
		ops[2].shape.group = {WordKind::Value, WordKind::Type};
		ops[2].sig.operands = 2;
		ops[2].sig.result = TypeId::I64;
		if (reversed) {
			std::reverse(ops.begin(), ops.end());
		}
		for (auto& o : ops) {
			o.hasExpansion = true;
			r.registerOp(o);
		}

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
	                   "extension \"test.splat@1\"\n"
	                   "extension \"test.shuffle@1\"\n"
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
	CHECK(r.module->extensions.size() == 5);
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

namespace {

const char* const program = "nir 1.0\n"
                            "callee #c0 = intrinsic test.fn : (i64) -> i64 attrs(noUnwind)\n"
                            "func export @f : (i64) -> i64 {\n"
                            "^b0(%0: i64):\n"
                            "  %1 = test.splat %0, 3 : i64\n"
                            "  %2 = test.shuffle [%0, 1, %1, i64] : i64\n"
                            "  %3 = call #c0(%2)\n"
                            "  ret %3\n"
                            "}\n";

LoadOptions loadOptions() {
	LoadOptions lo;
	lo.ignoreProcessToken = true;
	return lo;
}

} // namespace

TEST_CASE("an image holds image-local extension ids and loads under other process ids", "[image][extension]") {
	std::vector<uint8_t> image;
	std::string printed;
	Hash128 hash;
	{
		Fixture fixture;
		auto r = parse(program);
		REQUIRE(r.ok());
		printed = print(*r.module);
		image = writeImage(*r.module);
		hash = structuralHash(*r.module, effectiveOptions({}));
		// the image names the operations by their position among the entries of kind operation
		auto loaded = loadImage(image, loadOptions());
		INFO((loaded.error ? loaded.error->message : std::string()));
		REQUIRE(loaded.ok());
		CHECK(print(*loaded.module) == printed);
	}
	// the same plugin registered in another order: every process opcode differs
	Fixture reversed(true);
	auto& registry = ExtensionRegistry::instance();
	CHECK(*registry.findOp("test.shuffle") != 256 + 2);
	auto loaded = loadImage(image, loadOptions());
	INFO((loaded.error ? loaded.error->message : std::string()));
	REQUIRE(loaded.ok());
	CHECK(print(*loaded.module) == printed);
	CHECK(loaded.module->insts[1].op == *registry.findOp("test.splat"));
	CHECK(loaded.module->insts[2].op == *registry.findOp("test.shuffle"));
	CHECK(writeImage(*loaded.module) == image);
	// and the structural hash does not depend on the process ids
	auto again = parse(program);
	REQUIRE(again.ok());
	CHECK(structuralHash(*again.module, effectiveOptions({})) == hash);
}

TEST_CASE("the loader rejects missing extensions, versions out of range and intrinsic signature mismatches",
          "[image][extension]") {
	std::vector<uint8_t> image;
	{
		Fixture fixture;
		auto r = parse(program);
		REQUIRE(r.ok());
		image = writeImage(*r.module);
	}
	{
		// test.splat now needs version 2 or later
		Fixture newer(false, 2);
		auto loaded = loadImage(image, loadOptions());
		REQUIRE_FALSE(loaded.ok());
		CHECK(loaded.error->code == DiagCode::L011);
	}
	{
		ExtensionRegistryTestAccess::reset();
		auto loaded = loadImage(image, loadOptions());
		REQUIRE_FALSE(loaded.ok());
		CHECK(loaded.error->code == DiagCode::L010);
	}
	{
		Fixture fixture;
		auto r = parse(program);
		REQUIRE(r.ok());
		// the callee's signature differs from the registered (i64) -> i64
		auto m = std::move(*r.module);
		TypeWord params[] = {makeTypeWord(TypeId::I32)};
		m.callees[0].signature = m.internSignature(makeTypeWord(TypeId::I64), params);
		auto bad = writeImage(m);
		auto loaded = loadImage(bad, loadOptions());
		REQUIRE_FALSE(loaded.ok());
		INFO(loaded.error->message);
		CHECK(loaded.error->code == DiagCode::L012);
	}
}
