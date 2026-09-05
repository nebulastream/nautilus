// Nested regions, and where their names and source locations come from.
//
// A region (docs/region.md) marks a bounded part of a traced function as its own tracing
// scope. Every region records two attributes about the call site that opened it -- an
// optional name and the source location it was written at -- and those travel with the
// traced code: into the trace, into the IR, and into the error message if the body is
// malformed. This demo builds a three-level nesting and prints the dumps so you can see
// each stage of the pipeline naming the region its code came from.

#include <cstdint>
#include <iostream>
#include <nautilus/Engine.hpp>
#include <nautilus/region.hpp>
#include <source_location>
#include <utility>

using namespace nautilus;

/// A helper that opens a region for its caller.
///
/// Taking the location as a defaulted argument and handing it to region() through
/// RegionAttributes is what makes the recorded position point at the *caller* -- at the
/// `stage("accumulate", ...)` line below, rather than at this line, which is where the
/// plain `region(name, body)` overload would have pointed for every caller alike.
template <typename F>
void stage(const char* name, F&& body, std::source_location where = std::source_location::current()) {
	region(RegionAttributes {name, SourceLocation::from(where)}, std::forward<F>(body));
}

/// Sums the values above a threshold and counts them, in three nested regions:
///
///     scan                     the whole pass over the input
///       classify               the per-row decision
///         accumulate           the update, opened through the helper above
///
/// Every value the regions produce is carried out through a `val<T>` declared outside
/// them (`total`, `kept`): a value created inside a region body may not outlive it, and
/// assigning to an outer one is the way to hand a result back.
val<int64_t> summarize(val<int32_t> size, val<int32_t*> values, val<int32_t> threshold) {
	val<int64_t> total = 0;
	val<int32_t> kept = 0;

	region("scan", [&]() {
		for (val<int32_t> i = 0; i < size; i = i + 1) {
			val<int32_t> value = values[i];
			region("classify", [&]() {
				if (value > threshold) {
					stage("accumulate", [&]() {
						total = total + value;
						kept = kept + 1;
					});
				}
			});
		}
	});

	return total * 100 + kept;
}

int main(int, char*[]) {
	engine::Options options;
	// Regions are a lazyTracing feature -- the default tracer. Under
	// "exceptionBasedTracing" the bodies below are traced inline and no region appears
	// anywhere in the dumps.
	options.setOption("engine.backend", "cpp");
	// Print the trace and the Nautilus IR to the console so the region markers are
	// visible. `dump.file` would additionally write them to a temp folder.
	options.setOption("dump.after_tracing", true);
	options.setOption("dump.after_ir_creation", true);
	options.setOption("dump.console", true);
	options.setOption("dump.file", false);

	auto engine = engine::NautilusEngine(options);
	auto function = engine.registerFunction(summarize);

	auto values = new int32_t[6] {3, 14, 7, 1, 42, 9};
	// 14 + 42 + 9 = 65 above the threshold of 8, three values kept.
	auto result = function(6, values, 8);
	delete[] values;

	std::cout << "\nsummarize(threshold=8) = " << result << "  (total=" << result / 100
	          << ", kept=" << result % 100 << ")\n\n";

	std::cout << "What the dumps above show:\n"
	          << "  * every block of the trace that belongs to a region is preceded by\n"
	          << "      ; region \"name\" at DemoRegions.cpp:<line>:<column>\n"
	          << "  * in the IR a block states its region once, with the enclosing chain under\n"
	          << "    it, and an operation adds its own only where it came from deeper in the\n"
	          << "    nesting than the block it ended up in:\n"
	          << "      Block_N(...): ; region #2 \"accumulate\" at DemoRegions.cpp:<line>:<column>\n"
	          << "                    ; nested in region #1 \"classify\" ...\n"
	          << "                    ; nested in region #0 \"scan\" ...\n"
	          << "  * \"accumulate\" is named at its call site inside summarize(), not inside the\n"
	          << "    stage() helper that opened it -- that is what passing RegionAttributes does\n"
	          << "  * region ids (#0, #1, #2) are unique across the module; the index each block\n"
	          << "    and operation stores is only meaningful within its own function\n"
	          << "  * the regions leave no trace in the generated code: their entry and exit\n"
	          << "    blocks are seams the IR passes collapse, and no backend reads any of this\n";
	return 0;
}
