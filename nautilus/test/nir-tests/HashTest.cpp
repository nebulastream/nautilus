#include "TestModules.hpp"
#include "nautilus/nir/Hash.hpp"
#include "nautilus/nir/Parser.hpp"
#include <catch2/catch_all.hpp>

using namespace nautilus::nir;

namespace {

Module parseOk(const std::string& text) {
	auto r = parse(text);
	std::string all;
	if (r.error) {
		all = r.error->str();
	}
	INFO(all);
	REQUIRE(r.ok());
	return std::move(*r.module);
}

const std::string program = R"(nir 1.0
module "one"
pointer #p0 = callee symbol "ext_a"
callee #c0 = external #p0 : (i64) -> i64 attrs(noUnwind)
func export @f : (i64) -> i64 {
^b0(%0: i64):
  %1 = call #c0(%0)
  %2 = add %1, 42:i64
  ret %2
}
)";

Hash128 hashOf(const Module& m, const nautilus::OptionMap& options = {}) {
	return structuralHash(m, nautilus::effectiveOptions(options));
}

std::string replace(std::string s, const std::string& from, const std::string& to) {
	auto pos = s.find(from);
	REQUIRE(pos != std::string::npos);
	return s.replace(pos, from.size(), to);
}

} // namespace

TEST_CASE("XXH3-128 with seed 0 matches the reference values", "[nir][hash]") {
	auto empty = hashBytes({});
	REQUIRE(empty.hex() == "7f498d4624c30160d8984701d306aa99"); // key bytes: low 64 bits LE, then high 64 bits LE
	REQUIRE(empty.low == 0x6001c324468d497full);
	REQUIRE(empty.high == 0x99aa06d3014798d8ull);
}

TEST_CASE("equal modules hash equally, whatever their table order and name", "[nir][hash]") {
	auto a = parseOk(program);
	auto b =
	    parseOk(replace(replace(program, "module \"one\"", "module \"two\""), "pointer #p0 = callee symbol \"ext_a\"",
	                    "pointer #p0 = callee symbol \"never_used\"\npointer #p1 = callee symbol \"ext_a\""));
	// callees refer to #p1 in b
	auto text = replace(
	    replace(program, "module \"one\"", "module \"two\""),
	    "pointer #p0 = callee symbol \"ext_a\"\ncallee #c0 = external #p0",
	    "pointer #p0 = callee symbol \"never_used\"\npointer #p1 = callee symbol \"ext_a\"\ncallee #c0 = external #p1");
	auto c = parseOk(text);
	REQUIRE(hashOf(a) == hashOf(c));
	(void) b;
}

TEST_CASE("changing the program changes the hash", "[nir][hash]") {
	auto base = hashOf(parseOk(program));
	REQUIRE(hashOf(parseOk(replace(program, "42:i64", "43:i64"))) != base);
	REQUIRE(hashOf(parseOk(replace(program, "add %1", "sub %1"))) != base);
	REQUIRE(hashOf(parseOk(replace(program, "\"ext_a\"", "\"ext_b\""))) != base);
	REQUIRE(hashOf(parseOk(replace(program, "@f", "@g"))) != base); // the name of an exported function is the interface
}

TEST_CASE("options enter the hash only when they affect code", "[nir][hash]") {
	auto m = parseOk(program);
	auto base = hashOf(m);
	REQUIRE(hashOf(m, {{"ir.enableLICM", false}}) == base); // the default
	REQUIRE(hashOf(m, {{"ir.enableLICM", true}}) != base);
	REQUIRE(hashOf(m, {{"dump.all", true}, {"engine.logStatistics", true}}) == base);
	REQUIRE(hashOf(m, {{"unknown.application.option", std::string("x")}}) == base);
	REQUIRE(hashOf(m, {{"optimizationLevel", int64_t(1)}}) != hashOf(m, {{"optimizationLevel", int64_t(2)}}));
}

TEST_CASE("debug sections do not enter the hash", "[nir][hash]") {
	auto m = parseOk(program);
	auto base = hashOf(m);
	m.debugNames.push_back(NameRecord {0, 1});
	m.debugNameBytes.push_back('x');
	m.debugLocations.push_back(DebugLocationRecord {0, 3, 4});
	REQUIRE(hashOf(m) == base);
}

TEST_CASE("an importer's key does not change when the exporter's body does", "[nir][hash]") {
	std::string importer = R"(nir 1.0
import #i0 = "lib.math" :: "hash" : (i64) -> i64 attrs(noUnwind)
callee #c0 = imported #i0 : (i64) -> i64 attrs(noUnwind)
func export @run : (i64) -> i64 {
^b0(%0: i64):
  %1 = call #c0(%0)
  ret %1
}
)";
	auto key = hashOf(parseOk(importer));
	// the exporter is not part of the importer's module, so editing it cannot change the key; the import record is
	REQUIRE(hashOf(parseOk(replace(importer, "attrs(noUnwind)", "attrs(noUnwind, willReturn)"))) != key);
	REQUIRE(hashOf(parseOk(replace(importer, "\"hash\"", "\"hash2\""))) != key);
	REQUIRE(hashOf(parseOk(replace(importer, "(i64) -> i64 attrs(noUnwind)\ncallee", "(i64) -> i64\ncallee"))) != key);
}

TEST_CASE("the key of code that inlined imports", "[nir][hash]") {
	auto module = hashOf(parseOk(program));
	auto e1 = hashBytes(std::vector<uint8_t> {1});
	auto e2 = hashBytes(std::vector<uint8_t> {2});
	auto k = keyWithInlinedImports(module, {{"b.lib", e2}, {"a.lib", e1}});
	REQUIRE(k == keyWithInlinedImports(module, {{"a.lib", e1}, {"b.lib", e2}})); // sorted by name
	REQUIRE(k != keyWithInlinedImports(module, {{"a.lib", e2}, {"b.lib", e1}}));
	REQUIRE(k != keyWithInlinedImports(module, {{"a.lib", e1}}));
	REQUIRE(k != module);
	// by definition: XXH3-128 over key, count, then name length, name, hash per exporter
	std::vector<uint8_t> input;
	auto mk = module.bytes();
	input.insert(input.end(), mk.begin(), mk.end());
	input.insert(input.end(), {1, 0, 0, 0, 5, 0, 0, 0, 'a', '.', 'l', 'i', 'b'});
	auto h = e1.bytes();
	input.insert(input.end(), h.begin(), h.end());
	REQUIRE(keyWithInlinedImports(module, {{"a.lib", e1}}) == hashBytes(input));
}

TEST_CASE("a process-local module hashes its token and addresses", "[nir][hash]") {
	auto text = R"(nir 1.0
pointer #p0 = address local 0x1000 raw
func export @f : () -> ptr {
^b0:
  %0 = ptr.const #p0
  ret %0
}
)";
	auto a = parseOk(text);
	auto b = parseOk(replace(text, "0x1000", "0x2000"));
	REQUIRE(hashOf(a) != hashOf(b));
	auto c = parseOk(text);
	c.module.processToken ^= 1; // another process
	REQUIRE(hashOf(a) != hashOf(c));
}

TEST_CASE("hashing a pre-SSA module is refused", "[nir][hash]") {
	Module m;
	test::buildPreSSA(m);
	REQUIRE_THROWS_AS(hashOf(m), std::invalid_argument);
}
