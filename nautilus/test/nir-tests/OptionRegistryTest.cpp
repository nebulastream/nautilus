#include "nautilus/common/OptionRegistry.hpp"
#include <catch2/catch_all.hpp>
#include <fstream>
#include <regex>
#include <set>
#include <sstream>

using namespace nautilus;

namespace {

/// The options of the first table of docs/options.md with their Type and Code columns.
struct DocRow {
	std::string name, type, code;
};

std::vector<DocRow> docRows() {
	std::ifstream in(NIR_OPTIONS_DOC);
	std::string line;
	std::vector<DocRow> rows;
	bool table = false;
	while (std::getline(in, line)) {
		if (line.rfind("| Option ", 0) == 0) {
			table = true;
			continue;
		}
		if (!table) {
			continue;
		}
		if (line.rfind("|---", 0) == 0) {
			continue;
		}
		if (line.rfind("|", 0) != 0) {
			break;
		}
		std::vector<std::string> cells;
		std::stringstream ss(line);
		std::string cell;
		std::getline(ss, cell, '|'); // before the first |
		while (cells.size() < 5 && std::getline(ss, cell, '|')) {
			cells.push_back(cell);
		}
		auto trim = [](std::string s) {
			s.erase(0, s.find_first_not_of(' '));
			s.erase(s.find_last_not_of(' ') + 1);
			return s;
		};
		std::string first = trim(cells[0]);
		size_t pos = 0;
		while (true) {
			auto sep = first.find(" / ", pos);
			auto part = first.substr(pos, sep == std::string::npos ? std::string::npos : sep - pos);
			rows.push_back({part.substr(0, part.find_first_of("= ")), trim(cells[3]), trim(cells[4])});
			if (sep == std::string::npos) {
				break;
			}
			pos = sep + 3;
		}
	}
	return rows;
}

const OptionSpec& spec(std::string_view name) {
	for (auto& s : optionRegistry()) {
		if (s.name == name) {
			return s;
		}
	}
	throw std::logic_error("unregistered option");
}

} // namespace

TEST_CASE("every documented option is registered and every registered option is documented", "[nir][options]") {
	auto rows = docRows();
	REQUIRE(rows.size() > 50);
	std::set<std::string> documented;
	for (auto& r : rows) {
		documented.insert(r.name);
		bool alias = false;
		for (auto& a : deprecatedAliases()) {
			alias |= a.name == r.name;
		}
		if (alias) {
			continue;
		}
		INFO(r.name);
		const OptionSpec* s = nullptr;
		for (auto& x : optionRegistry()) {
			if (x.name == r.name) {
				s = &x;
			}
		}
		REQUIRE(s != nullptr);
		REQUIRE(r.code == (s->affectsCode ? "yes" : "no"));
		std::string type = s->type == OptionType::Bool     ? "bool"
		                   : s->type == OptionType::Int    ? "int"
		                   : s->type == OptionType::Double ? "double"
		                   : s->allowed.empty()            ? "string"
		                                                   : "enum";
		REQUIRE(r.type == type);
	}
	for (auto& s : optionRegistry()) {
		INFO(s.name);
		REQUIRE(documented.count(std::string(s.name)) == 1);
	}
	for (auto& a : deprecatedAliases()) {
		REQUIRE(documented.count(std::string(a.name)) == 1);
	}
}

TEST_CASE("effective options are the defaults overlaid with the set values", "[nir][options]") {
	auto e = effectiveOptions({});
	REQUIRE(std::get<bool>(e.values.at("engine.foldStaticConstants")) == true);
	REQUIRE(std::get<int64_t>(e.values.at("asmjit.inliningMaxOperations")) == 24);
	REQUIRE(e.values.count("optimizationLevel") == 0); // no default
	auto s = effectiveOptions({{"engine.foldStaticConstants", false}, {"optimizationLevel", int64_t(2)}, {"my.app", std::string("x")}});
	REQUIRE(std::get<bool>(s.values.at("engine.foldStaticConstants")) == false);
	REQUIRE(std::get<int64_t>(s.values.at("optimizationLevel")) == 2);
	REQUIRE(std::get<std::string>(s.values.at("my.app")) == "x"); // application options are kept
	REQUIRE(s.warnings.empty());
}

TEST_CASE("option values are type checked", "[nir][options]") {
	REQUIRE_THROWS_AS(effectiveOptions({{"debug", int64_t(1)}}), std::invalid_argument);
	REQUIRE_THROWS_AS(effectiveOptions({{"optimizationLevel", true}}), std::invalid_argument);
	REQUIRE_THROWS_AS(effectiveOptions({{"tbc.mode", std::string("bogus")}}), std::invalid_argument);
	REQUIRE_NOTHROW(effectiveOptions({{"tbc.mode", std::string("jit")}}));
	REQUIRE(std::get<bool>(parseOptionValue(spec("debug"), "true")));
	REQUIRE(std::get<int64_t>(parseOptionValue(spec("tbc.stackSizeKb"), "2048")) == 2048);
	REQUIRE(std::get<std::string>(parseOptionValue(spec("tbc.dispatch"), "goto")) == "goto");
	REQUIRE_THROWS_AS(parseOptionValue(spec("debug"), "yes"), std::invalid_argument);
	REQUIRE_THROWS_AS(parseOptionValue(spec("tbc.stackSizeKb"), "12x"), std::invalid_argument);
	REQUIRE_THROWS_AS(parseOptionValue(spec("tbc.dispatch"), "jump"), std::invalid_argument);
}

TEST_CASE("the deprecated alias maps onto engine.tiered.promotion", "[nir][options]") {
	auto off = effectiveOptions({{"engine.tiered.backgroundPromotion", false}});
	REQUIRE(std::get<std::string>(off.values.at("engine.tiered.promotion")) == "none");
	REQUIRE(off.warnings.size() == 1);
	auto on = effectiveOptions({{"engine.tiered.backgroundPromotion", true}});
	REQUIRE(std::get<std::string>(on.values.at("engine.tiered.promotion")) == "eager");
	REQUIRE(on.warnings.size() == 1);
	// when both are set the alias wins, with a second warning
	auto both = effectiveOptions({{"engine.tiered.backgroundPromotion", false}, {"engine.tiered.promotion", std::string("manual")}});
	REQUIRE(std::get<std::string>(both.values.at("engine.tiered.promotion")) == "none");
	REQUIRE(both.warnings.size() == 2);
}

TEST_CASE("the options block of the hash covers code-affecting options only", "[nir][options]") {
	auto block = [](const OptionMap& m) { return encodeOptionsBlock(effectiveOptions(m)); };
	auto base = block({});
	// setting an option to its default does not change the block
	REQUIRE(block({{"ir.enableLICM", false}, {"asmjit.inliningMaxOperations", int64_t(24)}}) == base);
	// a code-affecting option does
	REQUIRE(block({{"ir.enableLICM", true}}) != base);
	REQUIRE(block({{"optimizationLevel", int64_t(0)}}) != base);
	// options without affectsCode, application options and aliases of them do not
	REQUIRE(block({{"dump.all", true}, {"engine.logStatistics", true}, {"tbc.stackSizeKb", int64_t(1)}}) == base);
	REQUIRE(block({{"my.app.option", std::string("x")}}) == base);
	// the block is deterministic: u32 count, then sorted entries
	uint32_t count = base[0] | (base[1] << 8) | (base[2] << 16) | (uint32_t(base[3]) << 24);
	size_t affecting = 0;
	for (auto& s : optionRegistry()) {
		affecting += s.affectsCode && !s.defaultText.empty();
	}
	REQUIRE(count == affecting);
}

TEST_CASE("encoding of values", "[nir][options]") {
	std::vector<OptionSpec> reg = {{"a.bool", OptionType::Bool, "true", true},
	                               {"b.int", OptionType::Int, "-2", true},
	                               {"c.double", OptionType::Double, "1.5", true},
	                               {"d.string", OptionType::String, "hi", true},
	                               {"e.skipped", OptionType::Bool, "true", false}};
	auto e = effectiveOptions({}, reg, {});
	auto b = encodeOptionsBlock(e, reg);
	std::vector<uint8_t> expected = {4, 0, 0, 0,                                             // count
	                                 6, 0, 0, 0, 'a', '.', 'b', 'o', 'o', 'l', 0, 1,           // bool
	                                 5, 0, 0, 0, 'b', '.', 'i', 'n', 't', 1, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	                                 8, 0, 0, 0, 'c', '.', 'd', 'o', 'u', 'b', 'l', 'e', 2, 0, 0, 0, 0, 0, 0, 0xF8, 0x3F,
	                                 8, 0, 0, 0, 'd', '.', 's', 't', 'r', 'i', 'n', 'g', 3, 2, 0, 0, 0, 'h', 'i'};
	REQUIRE(b == expected);
}

TEST_CASE("a resolve hook maps the effective value before it is hashed", "[nir][options]") {
	std::vector<OptionSpec> reg = {{"gpu.device", OptionType::String, "auto", true, {}, [](const OptionValue& v) {
		                                return std::get<std::string>(v) == "auto" ? OptionValue(std::string("cuda")) : v;
	                                }}};
	auto automatic = encodeOptionsBlock(effectiveOptions({}, reg, {}), reg);
	auto cuda = encodeOptionsBlock(effectiveOptions({{"gpu.device", std::string("cuda")}}, reg, {}), reg);
	auto none = encodeOptionsBlock(effectiveOptions({{"gpu.device", std::string("none")}}, reg, {}), reg);
	REQUIRE(automatic == cuda);
	REQUIRE(automatic != none);
}
