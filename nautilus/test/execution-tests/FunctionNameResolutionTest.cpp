#include "nautilus/function.hpp"
#include "nautilus/options.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/TraceContext.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>
#include <functional>
#include <optional>
#include <string>

using namespace nautilus;
using nautilus::engine::Options;
using nautilus::tracing::ExecutionTrace;
using nautilus::tracing::FunctionCall;

namespace {

int addInts(int lhs, int rhs) {
        return lhs + rhs;
}

std::function<void()> makeTraceableCall() {
        return []() {
                auto a = val<int>(1);
                auto b = val<int>(2);
                auto result = invoke(addInts, a, b);
                tracing::traceReturnOperation(tracing::TypeResolver<int>::to_type(), result.state);
        };
}

std::optional<FunctionCall> extractFunctionCall(ExecutionTrace& trace) {
        for (auto& block : trace.getBlocks()) {
                for (auto& operation : block.operations) {
                        for (const auto& input : operation.input) {
                                if (auto call = std::get_if<FunctionCall>(&input)) {
                                        return *call;
                                }
                        }
                }
        }

        return std::nullopt;
}

} // namespace

#ifdef ENABLE_TRACING
TEST_CASE("function names resolve with and without DWARF support") {
        auto traceableCall = makeTraceableCall();

        SECTION("fallback resolution without DWARF") {
                Options options;
                options.setOption("engine.resolveFunctionNamesWithDwarf", false);

                auto trace = tracing::TraceContext::trace(traceableCall, options);
                auto call = extractFunctionCall(*trace);

                REQUIRE(call.has_value());
                REQUIRE(call->functionName.find("addInts") != std::string::npos);
                REQUIRE_FALSE(call->location.has_value());
        }

        SECTION("DWARF resolution when available") {
                Options options;
                options.setOption("engine.resolveFunctionNamesWithDwarf", true);

                auto trace = tracing::TraceContext::trace(traceableCall, options);
                auto call = extractFunctionCall(*trace);

                REQUIRE(call.has_value());
                REQUIRE(call->functionName.find("addInts") != std::string::npos);
#if BACKWARD_HAS_DW == 1
                if (call->location.has_value()) {
                        REQUIRE(call->location->file.find("FunctionNameResolutionTest.cpp") != std::string::npos);
                } else {
                        WARN("DWARF metadata unavailable; falling back to non-DWARF resolution.");
                }
#else
                REQUIRE_FALSE(call->location.has_value());
#endif
        }
}
#endif
