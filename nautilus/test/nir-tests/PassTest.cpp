// The optional passes (docs/plans/nautilus-0.4/03-conversion-and-passes.md §5, plan M3.8): LICM, strength reduction and
// inlining as copy passes, and the pass manager that runs them.
#include "nautilus/nir/Parser.hpp"
#include "nautilus/nir/Printer.hpp"
#include "nautilus/nir/Verifier.hpp"
#include "nautilus/passes/Cfg.hpp"
#include "nautilus/passes/Passes.hpp"
#include <algorithm>
#include <catch2/catch_all.hpp>
#include <map>

using namespace nautilus;
using nir::Module;
using nir::Op;

namespace {

Module parseOk(const std::string& text) {
	auto r = nir::parse(text);
	std::string all;
	if (r.error) {
		all += r.error->str() + "\n";
	}
	for (auto& d : r.verifierDiagnostics) {
		all += d.str() + "\n";
	}
	INFO(all);
	REQUIRE(r.ok());
	return std::move(*r.module);
}

void requireValid(const Module& m) {
	std::string all;
	for (auto& d : nir::verify(m)) {
		all += d.str() + "\n";
	}
	INFO(all + nir::print(m));
	REQUIRE(all.empty());
}

struct Outcome {
	bool changed = false;
	uint64_t changes = 0;
	Module module;
	std::vector<std::string> notes;
};

/// Runs `pass` on the module of `text`; the result must verify.
Outcome run(std::unique_ptr<passes::Pass> pass, const std::string& text, const passes::ImportView* imports = nullptr) {
	auto in = parseOk(text);
	engine::Options options;
	Outcome outcome;
	passes::PassContext context {options};
	context.imports = imports;
	context.notes = &outcome.notes;
	auto result = pass->run(in, context);
	outcome.changed = result.has_value();
	outcome.changes = context.changes;
	outcome.module = result ? std::move(*result) : std::move(in);
	requireValid(outcome.module);
	if (std::getenv("NIR_PASS_PRINT") != nullptr) { // NIR_PASS_PRINT=1 ctest -V -R <test>: the module after the pass
		WARN(nir::print(outcome.module));
	}
	return outcome;
}

uint32_t functionNamed(const Module& m, std::string_view name) {
	for (uint32_t f = 0; f < m.functions.size(); ++f) {
		if (m.name(m.functions[f].nameId) == name) {
			return f;
		}
	}
	FAIL("no function " << name);
	return nir::NONE;
}

/// The opcodes of a block of a function, in order.
std::vector<Op> ops(const Module& m, std::string_view function, uint32_t block) {
	const uint32_t f = functionNamed(m, function);
	const auto blocks = m.functionBlocks(f);
	std::vector<Op> result;
	for (uint32_t i = blocks[block].first; i < blocks[block].first + blocks[block].count; ++i) {
		result.push_back(static_cast<Op>(m.functionInsts(f)[i].op));
	}
	return result;
}

size_t count(const Module& m, std::string_view function, Op op) {
	const uint32_t f = functionNamed(m, function);
	size_t n = 0;
	for (const auto& inst : m.functionInsts(f)) {
		n += inst.op == static_cast<uint16_t>(op) ? 1 : 0;
	}
	return n;
}

bool contains(const std::vector<Op>& list, Op op) {
	return std::find(list.begin(), list.end(), op) != list.end();
}

const char* INVARIANT = R"(nir 1.0

func export @f : (i64, i64, i64) -> i64 {
^b0(%0: i64, %1: i64, %2: i64):
  br ^b1(0:i64, 0:i64)
^b1(%4: i64, %5: i64):
  %6 = cmp.lt.s %4, %0
  condbr %6, ^b2, ^b3(%5)
^b2:
  %8 = mul %1, %2
  %9 = add %8, %4
  %10 = add %5, %9
  %11 = add %4, 1:i64
  br ^b1(%11, %10)
^b3(%13: i64):
  ret %13
}
)";

} // namespace

TEST_CASE("LICM hoists an invariant product into the preheader that falls through", "[nir][passes][licm]") {
	auto r = run(passes::makeLicmPass(), INVARIANT);
	REQUIRE(r.changed);
	REQUIRE(r.changes == 1);
	// the preheader is ^b0, which ends in an unconditional branch; the body keeps the two adds and the increment
	REQUIRE(contains(ops(r.module, "f", 0), Op::Mul));
	REQUIRE_FALSE(contains(ops(r.module, "f", 2), Op::Mul));
	REQUIRE(r.module.functions[0].blockCount == 4);
	INFO(nir::print(r.module));
}

TEST_CASE("LICM leaves a module without an invariant alone", "[nir][passes][licm]") {
	auto r = run(passes::makeLicmPass(), R"(nir 1.0

func export @f : (i64) -> i64 {
^b0(%0: i64):
  br ^b1(0:i64, 0:i64)
^b1(%2: i64, %3: i64):
  %4 = cmp.lt.s %2, %0
  condbr %4, ^b2, ^b3(%3)
^b2:
  %6 = mul %2, %2
  %7 = add %3, %6
  %8 = add %2, 1:i64
  br ^b1(%8, %7)
^b3(%10: i64):
  ret %10
}
)");
	REQUIRE_FALSE(r.changed);
	REQUIRE(r.changes == 0);
}

TEST_CASE("LICM makes a preheader when the loop is entered through a branch", "[nir][passes][licm]") {
	auto r = run(passes::makeLicmPass(), R"(nir 1.0

func export @f : (i64, i64) -> i64 {
^b0(%0: i64, %1: i64):
  %2 = cmp.gt.s %0, 0:i64
  condbr %2, ^b1(0:i64, 0:i64), ^b2(-1:i64)
^b1(%4: i64, %5: i64):
  %6 = mul %1, 3:i64
  %7 = add %5, %6
  %8 = add %4, 1:i64
  %9 = cmp.lt.s %8, %0
  condbr %9, ^b1(%8, %7), ^b2(%7)
^b2(%11: i64):
  ret %11
}
)");
	REQUIRE(r.changed);
	// one block more: the preheader sits between the entry block and the loop and holds the product
	REQUIRE(r.module.functions[0].blockCount == 4);
	REQUIRE(ops(r.module, "f", 1).size() == 4); // two block arguments' worth: mul, br
	REQUIRE(contains(ops(r.module, "f", 1), Op::Mul));
	REQUIRE(ops(r.module, "f", 1).back() == Op::Br);
	REQUIRE_FALSE(contains(ops(r.module, "f", 2), Op::Mul));
}

TEST_CASE("LICM does not move loads, stores, checked divisions or calls with effects", "[nir][passes][licm]") {
	auto r = run(passes::makeLicmPass(), R"(nir 1.0
pointer #p0 = callee symbol "f"
pointer #p1 = callee symbol "g"
callee #c0 = external #p0 : (i64) -> i64 attrs(noUnwind)
callee #c1 = external #p1 : (i64) -> i64 attrs(noArgRead, noArgWrite, noOtherRead, noOtherWrite, noUnwind, willReturn)

func export @f : (ptr, i64, i64) -> i64 {
^b0(%0: ptr, %1: i64, %2: i64):
  br ^b1(0:i64, 0:i64)
^b1(%4: i64, %5: i64):
  %6 = cmp.lt.s %4, %1
  condbr %6, ^b2, ^b3(%5)
^b2:
  %8 = load %0 : i64
  %9 = call #c0(%2)
  %10 = div.s.checked %1, %2 unwind caller
  %11 = call #c1(%2)
  %12 = add %5, %8
  %13 = add %12, %9
  %14 = add %13, %10
  %15 = add %14, %11
  %16 = add %4, 1:i64
  br ^b1(%16, %15)
^b3(%18: i64):
  ret %18
}
)");
	// only the call of the pure callee is hoisted
	REQUIRE(r.changed);
	REQUIRE(r.changes == 1);
	REQUIRE(count(r.module, "f", Op::Call) == 2);
	REQUIRE(contains(ops(r.module, "f", 0), Op::Call));
	REQUIRE(contains(ops(r.module, "f", 2), Op::Load));
	REQUIRE(contains(ops(r.module, "f", 2), Op::Div));
}

TEST_CASE("LICM hoists out of both loops of a nest", "[nir][passes][licm]") {
	auto r = run(passes::makeLicmPass(), R"(nir 1.0

func export @f : (i64, i64, i64) -> i64 {
^b0(%0: i64, %1: i64, %2: i64):
  br ^b1(0:i64, 0:i64)
^b1(%4: i64, %5: i64):
  %6 = cmp.lt.s %4, %0
  condbr %6, ^b2, ^b6(%5)
^b2:
  br ^b3(0:i64, %5)
^b3(%9: i64, %10: i64):
  %11 = cmp.lt.s %9, %1
  condbr %11, ^b4, ^b5
^b4:
  %13 = mul %2, %2
  %14 = add %10, %13
  %15 = add %14, %9
  %16 = add %9, 1:i64
  br ^b3(%16, %15)
^b5:
  %18 = add %4, 1:i64
  br ^b1(%18, %10)
^b6(%20: i64):
  ret %20
}
)");
	REQUIRE(r.changed);
	// the product depends on a parameter only: it ends in the entry block, outside both loops
	REQUIRE(contains(ops(r.module, "f", 0), Op::Mul));
	REQUIRE(count(r.module, "f", Op::Mul) == 1);
}

TEST_CASE("strength reduction makes the element address a loop-carried pointer", "[nir][passes][strength]") {
	auto r = run(passes::makeStrengthReductionPass(), R"(nir 1.0

func export @f : (ptr, i64) -> i64 {
^b0(%0: ptr, %1: i64):
  br ^b1(0:i64, 0:i64)
^b1(%3: i64, %4: i64):
  %5 = cmp.lt.s %3, %1
  condbr %5, ^b2, ^b3(%4)
^b2:
  %7 = mul %3, 8:i64
  %8 = add %0, %7
  %9 = load %8 : i64
  %10 = add %4, %9
  %11 = add %3, 1:i64
  br ^b1(%11, %10)
^b3(%13: i64):
  ret %13
}
)");
	REQUIRE(r.changed);
	REQUIRE(r.changes == 1);
	// the multiplication left the loop body; the header has a pointer argument
	REQUIRE_FALSE(contains(ops(r.module, "f", 2), Op::Mul));
	REQUIRE(r.module.functionBlocks(0)[1].argCount == 3);
	INFO(nir::print(r.module));
}

TEST_CASE("strength reduction needs a counted 64-bit loop and a multiplication used once", "[nir][passes][strength]") {
	// the multiplication has a second use
	auto twice = run(passes::makeStrengthReductionPass(), R"(nir 1.0

func export @f : (ptr, i64) -> i64 {
^b0(%0: ptr, %1: i64):
  br ^b1(0:i64, 0:i64)
^b1(%3: i64, %4: i64):
  %5 = cmp.lt.s %3, %1
  condbr %5, ^b2, ^b3(%4)
^b2:
  %7 = mul %3, 8:i64
  %8 = add %0, %7
  %9 = load %8 : i64
  %10 = add %9, %7
  %11 = add %4, %10
  %12 = add %3, 1:i64
  br ^b1(%12, %11)
^b3(%14: i64):
  ret %14
}
)");
	REQUIRE_FALSE(twice.changed);
	// the induction variable is stepped by a value that is not a constant
	auto variable = run(passes::makeStrengthReductionPass(), R"(nir 1.0

func export @f : (ptr, i64, i64) -> i64 {
^b0(%0: ptr, %1: i64, %2: i64):
  br ^b1(0:i64, 0:i64)
^b1(%4: i64, %5: i64):
  %6 = cmp.lt.s %4, %1
  condbr %6, ^b2, ^b3(%5)
^b2:
  %8 = mul %4, 8:i64
  %9 = add %0, %8
  %10 = load %9 : i64
  %11 = add %5, %10
  %12 = add %4, %2
  br ^b1(%12, %11)
^b3(%14: i64):
  ret %14
}
)");
	REQUIRE_FALSE(variable.changed);
}

const char* LEAF = R"(nir 1.0
callee #c0 = internal @twice : (i64) -> i64

func export @f : (i64) -> i64 {
^b0(%0: i64):
  %1 = call #c0(%0)
  %2 = call #c0(%1)
  ret %2
}
func @twice : (i64) -> i64 {
^b0(%0: i64):
  %1 = add %0, %0
  ret %1
}
)";

TEST_CASE("inlining replaces a call of a leaf by its body and drops the callee", "[nir][passes][inline]") {
	auto r = run(passes::makeInlinePass({}), LEAF);
	REQUIRE(r.changed);
	REQUIRE(r.changes == 2);
	// one block, no call, and the function nothing refers to any more is gone
	REQUIRE(r.module.functions.size() == 1);
	REQUIRE(r.module.functions[0].blockCount == 1);
	REQUIRE(count(r.module, "f", Op::Call) == 0);
	REQUIRE(count(r.module, "f", Op::Add) == 2);
	REQUIRE(r.module.callees.empty());
}

TEST_CASE("inlining honors the size limit and leaves recursion alone", "[nir][passes][inline]") {
	passes::InlineConfig small;
	small.maxInsts = 2; // `twice` has three instructions
	auto tooLarge = run(passes::makeInlinePass(small), LEAF);
	REQUIRE_FALSE(tooLarge.changed);
	REQUIRE_FALSE(tooLarge.notes.empty());

	auto recursive = run(passes::makeInlinePass({}), R"(nir 1.0
callee #c0 = internal @sum : (i64) -> i64

func export @sum : (i64) -> i64 {
^b0(%0: i64):
  %1 = cmp.le.s %0, 0:i64
  condbr %1, ^b1, ^b2
^b1:
  ret 0:i64
^b2:
  %4 = sub %0, 1:i64
  %5 = call #c0(%4)
  %6 = add %5, %0
  ret %6
}
)");
	REQUIRE_FALSE(recursive.changed);
}

TEST_CASE("inlining a callee with several returns joins them in a block argument", "[nir][passes][inline]") {
	auto r = run(passes::makeInlinePass({}), R"(nir 1.0
callee #c0 = internal @abs : (i64) -> i64

func export @f : (i64) -> i64 {
^b0(%0: i64):
  %1 = call #c0(%0)
  %2 = add %1, 1:i64
  ret %2
}
func @abs : (i64) -> i64 {
^b0(%0: i64):
  %1 = cmp.lt.s %0, 0:i64
  condbr %1, ^b1, ^b2
^b1:
  %3 = sub 0:i64, %0
  ret %3
^b2:
  ret %0
}
)");
	REQUIRE(r.changed);
	REQUIRE(r.module.functions.size() == 1);
	REQUIRE(count(r.module, "f", Op::Call) == 0);
	// entry, the two arms and the continuation with the result as its argument
	REQUIRE(r.module.functions[0].blockCount == 4);
	REQUIRE(r.module.functionBlocks(0)[3].argCount == 1);
}

TEST_CASE("inlining keeps a callee that is exported or whose address is taken", "[nir][passes][inline]") {
	auto r = run(passes::makeInlinePass({}), R"(nir 1.0
callee #c0 = internal @twice : (i64) -> i64

func export @f : (i64) -> ptr {
^b0(%0: i64):
  %1 = call #c0(%0)
  %2 = funcaddr #c0
  ret %2
}
func @twice : (i64) -> i64 {
^b0(%0: i64):
  %1 = add %0, %0
  ret %1
}
)");
	REQUIRE(r.changed);
	REQUIRE(r.module.functions.size() == 2);
	REQUIRE(count(r.module, "f", Op::Call) == 0);
	REQUIRE(count(r.module, "f", Op::FuncAddr) == 1);
}

namespace {
const char* PADS = R"(nir 1.0
pointer #p0 = callee symbol "rt_throw"
pointer #p1 = thunk symbol "rt_throw$thunk"
pointer #p2 = destructor symbol "rt_destroy"
callee #c0 = external #p0 : (i64) -> void
callee #c1 = external #p2 : (ptr) -> void attrs(noUnwind)
callee #c2 = internal @inner : (i64) -> void

func @inner : (i64) -> void {
  alloca 8 align 8
^b0(%0: i64):
  %1 = alloca 0
  store %1, 21:i64
  call #c0(%0) unwind ^b1 thunk #p1
  call #c1(%1)
  ret
^b1 pad:
  call #c1(%1)
  resume
}
func export @outer : (i64) -> void {
  alloca 8 align 8
^b0(%0: i64):
  %1 = alloca 0
  store %1, 7:i64
  call #c2(%0) unwind ^b1
  call #c1(%1)
  ret
^b1 pad:
  call #c1(%1)
  resume
}
)";
}

TEST_CASE("inlining a callee with a landing pad continues its unwinding in the caller's pad", "[nir][passes][inline]") {
	auto r = run(passes::makeInlinePass({}), PADS);
	REQUIRE(r.changed);
	REQUIRE(r.module.functions.size() == 1);
	// the caller's alloca and the callee's
	REQUIRE(r.module.functions[0].allocaCount == 2);
	// two pads: the callee's ends in a branch to the caller's, which resumes
	const auto blocks = r.module.functionBlocks(0);
	size_t pads = 0, resumes = 0;
	for (uint32_t b = 0; b < blocks.size(); ++b) {
		pads += (blocks[b].flags & nir::BLOCK_LANDING_PAD) ? 1 : 0;
	}
	for (const auto& inst : r.module.functionInsts(0)) {
		resumes += inst.op == static_cast<uint16_t>(Op::Resume) ? 1 : 0;
	}
	REQUIRE(pads == 2);
	REQUIRE(resumes == 1);
	INFO(nir::print(r.module));
}

TEST_CASE("inlining a throwing callee into a caller without a handler keeps the unwinding", "[nir][passes][inline]") {
	auto r = run(passes::makeInlinePass({}), R"(nir 1.0
pointer #p0 = callee symbol "rt_throw"
pointer #p1 = thunk symbol "rt_throw$thunk"
callee #c0 = external #p0 : (i64) -> void
callee #c1 = internal @thrower : (i64) -> void

func @thrower : (i64) -> void {
^b0(%0: i64):
  call #c0(%0) unwind caller thunk #p1
  ret
}
func export @outer : (i64) -> void {
^b0(%0: i64):
  call #c1(%0) unwind caller
  ret
}
)");
	REQUIRE(r.changed);
	REQUIRE(r.module.functions.size() == 1);
	REQUIRE(count(r.module, "outer", Op::Call) == 1);
}

namespace {

/// An exporter whose module is a text, for the inlining across modules.
class FakeImports final : public passes::ImportView {
public:
	explicit FakeImports(std::shared_ptr<const Module> exporter, std::string name = "lib")
	    : module_(std::move(exporter)), name_(std::move(name)) {
	}
	Exporter exporter(const Module&, uint32_t) const override {
		return Exporter {module_, name_, std::nullopt};
	}

private:
	std::shared_ptr<const Module> module_;
	std::string name_;
};

const char* IMPORTER = R"(nir 1.0
import #i0 = "lib" :: "twice" : (i64) -> i64
callee #c0 = imported #i0 : (i64) -> i64

func export @f : (i64) -> i64 {
^b0(%0: i64):
  %1 = call #c0(%0)
  %2 = call #c0(%1)
  ret %2
}
)";

} // namespace

TEST_CASE("inlining copies the body of an imported callee, with what it calls, into the importer",
          "[nir][passes][inline]") {
	auto exporter = std::make_shared<Module>(parseOk(R"(nir 1.0 module "lib"
pointer #p0 = callee symbol "ext"
callee #c0 = internal @helper : (i64) -> i64
callee #c1 = external #p0 : (i64) -> i64 attrs(noUnwind)

func export @twice : (i64) -> i64 {
^b0(%0: i64):
  %1 = call #c0(%0)
  %2 = call #c1(%1)
  ret %2
}
func @helper : (i64) -> i64 {
^b0(%0: i64):
  %1 = add %0, %0
  ret %1
}
)"));
	FakeImports imports(exporter);
	std::vector<passes::PassContext::InlinedImport> inlined;
	auto in = parseOk(IMPORTER);
	engine::Options options;
	passes::PassContext context {options};
	context.imports = &imports;
	context.inlinedImports = &inlined;
	auto pass = passes::makeInlinePass({});
	auto result = pass->run(in, context);
	REQUIRE(result.has_value());
	requireValid(*result);
	INFO(nir::print(*result));
	// the exporter's body is part of the importer now and no call goes through the import slot
	REQUIRE(count(*result, "f", Op::Call) == 2); // the external call of each copy
	REQUIRE(result->functions.size() == 1);
	REQUIRE(inlined.size() == 1);
	REQUIRE(inlined[0].module == "lib");
	REQUIRE(inlined[0].function == "twice");
}

TEST_CASE("a function that uses bind.addr is not inlined across modules", "[nir][passes][inline]") {
	auto exporter = std::make_shared<Module>(parseOk(R"(nir 1.0 module "lib"
binding #b0 = "counter" : i64 size 8 align 8

func export @twice : (i64) -> i64 {
^b0(%0: i64):
  %1 = bind.addr #b0
  %2 = load %1 : i64
  %3 = add %2, %0
  ret %3
}
)"));
	FakeImports imports(exporter);
	auto r = run(passes::makeInlinePass({}), IMPORTER, &imports);
	REQUIRE_FALSE(r.changed);
	bool explained = false;
	for (const auto& n : r.notes) {
		explained = explained || n.find("bind.addr") != std::string::npos;
	}
	REQUIRE(explained);
}

TEST_CASE("the pass manager runs a group until nothing changes and reports every pass", "[nir][passes][manager]") {
	passes::PassManager manager;
	std::vector<std::unique_ptr<passes::Pass>> group;
	group.push_back(passes::makeInlinePass({}));
	group.push_back(passes::makeLicmPass());
	manager.addGroup(std::move(group), 4);
	auto in = parseOk(LEAF);
	engine::Options options;
	passes::PassContext context {options};
	passes::PipelineReport report;
	auto out = manager.run(in, context, report);
	REQUIRE(out.has_value());
	// round 1 changes the module, round 2 changes nothing
	REQUIRE(report.iterations == 2);
	REQUIRE(report.passes.size() == 2);
	REQUIRE(report.passes[0].name == "inline");
	REQUIRE(report.passes[0].runs == 2);
	REQUIRE(report.passes[0].changes == 2);
	REQUIRE(report.passes[1].name == "licm");
	REQUIRE(report.passes[1].runs == 2);
	REQUIRE(report.passes[1].changes == 0);
}

TEST_CASE("the optional passes of the options", "[nir][passes][options]") {
	engine::Options none;
	REQUIRE_FALSE(passes::optionalPassesOf(none).any());
	engine::Options options;
	options.setOption("ir.inline", true);
	options.setOption("ir.inline.maxInsts", 12);
	options.setOption("ir.enableLICM", true);
	auto which = passes::optionalPassesOf(options);
	REQUIRE(which.inlining);
	REQUIRE(which.licm);
	REQUIRE_FALSE(which.strengthReduction);
	REQUIRE(which.inlineConfig.maxInsts == 12);
	auto group = passes::makeOptionalPasses(which);
	REQUIRE(group.size() == 2);
	REQUIRE(group[0]->name() == "inline");
	REQUIRE(group[1]->name() == "licm");
}
