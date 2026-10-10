#include "CApiInternal.hpp"
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"

namespace nautilus::capi {

namespace {

struct LastError {
	NautilusStatus status = NAUTILUS_OK;
	std::string message;
};

LastError& lastError() noexcept {
	thread_local LastError error;
	return error;
}

} // namespace

void setError(NautilusStatus status, std::string message) noexcept {
	auto& error = lastError();
	error.status = status;
	// Moving a string cannot throw, so recording an error never fails.
	error.message = std::move(message);
}

NautilusStatus lastErrorStatus() noexcept {
	return lastError().status;
}

void prepareForPasses(NautilusIROpaqueGraph* graph) {
	check(graph->builders.empty(), NAUTILUS_ERROR_INVALID_STATE,
	      "every function builder must be finished or disposed first");
	compiler::ir::rebuildPredecessorLists(*graph->ir);
	if (graph->optimized) {
		return;
	}
	const auto result = compiler::ir::IRVerifier::verify(*graph->ir);
	if (!result.ok()) {
		throw ApiError(NAUTILUS_ERROR_VERIFICATION_FAILED, result.toString());
	}
}

} // namespace nautilus::capi

using namespace nautilus::capi;

namespace {

/// Sets option @p name to what @p value produces, which may itself throw.
template <typename F>
NautilusStatus setOption(NautilusOptionsRef options, NautilusStringRef name, F&& value) {
	return status([&] {
		require(options != nullptr, "options is NULL");
		const auto key = toString(name);
		require(!key.empty(), "option name is empty");
		options->options.setOption(key, value());
	});
}

} // namespace

extern "C" {

uint32_t nautilus_c_api_version(void) {
	return NAUTILUS_C_API_VERSION;
}

void nautilus_string_dispose(NautilusString str) {
	std::free(str.data);
}

NautilusStatus nautilus_last_error_code(void) {
	return lastError().status;
}

NautilusStringRef nautilus_last_error_message(void) {
	return borrow(lastError().message);
}

NautilusOptionsRef nautilus_options_create(void) {
	return guarded<NautilusOptionsRef>(nullptr, [] { return new NautilusOpaqueOptions(); });
}

void nautilus_options_dispose(NautilusOptionsRef options) {
	delete options;
}

NautilusStatus nautilus_options_set_bool(NautilusOptionsRef options, NautilusStringRef name, bool value) {
	return setOption(options, name, [&] { return value; });
}

NautilusStatus nautilus_options_set_int(NautilusOptionsRef options, NautilusStringRef name, int32_t value) {
	return setOption(options, name, [&] { return static_cast<int>(value); });
}

NautilusStatus nautilus_options_set_double(NautilusOptionsRef options, NautilusStringRef name, double value) {
	return setOption(options, name, [&] { return value; });
}

NautilusStatus nautilus_options_set_string(NautilusOptionsRef options, NautilusStringRef name,
                                           NautilusStringRef value) {
	return setOption(options, name, [&] { return toString(value); });
}

void nautilus_executable_dispose(NautilusExecutableRef executable) {
	delete executable;
}

NautilusStatus nautilus_executable_get_function(NautilusExecutableRef executable, NautilusStringRef name,
                                                NautilusFunctionPointer* out) {
	return status([&] {
		require(executable != nullptr, "executable is NULL");
		requireOut(out);
		const auto member = toString(name);
		void* function = nullptr;
		try {
			function = executable->executable->getInvocableFunctionPtr(member);
		} catch (const std::exception&) {
			// Some backends throw for an unknown name, others return NULL.
		}
		check(function != nullptr, NAUTILUS_ERROR_NOT_FOUND, "no compiled function with this name");
		*out = reinterpret_cast<NautilusFunctionPointer>(function);
	});
}

} // extern "C"
