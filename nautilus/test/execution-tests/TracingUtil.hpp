#pragma once

#include "nautilus/Engine.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace nautilus::engine { namespace details {
// old tracing functions for functions pointers
template <size_t... Indices, typename R, typename... FunctionArguments>
std::function<void()> createFunctionWrapper(std::index_sequence<Indices...>, R (*fnptr)(FunctionArguments...)) {
	[[maybe_unused]] std::size_t args = sizeof...(FunctionArguments);
	auto traceFunc = [=]() {
		if constexpr (std::is_void_v<R>) {
			fnptr(details::createTraceableArgument<FunctionArguments, Indices>()...);
			tracing::traceReturnOperation(Type::v, tracing::value_ref());
		} else {
			auto returnValue = fnptr(details::createTraceableArgument<FunctionArguments, Indices>()...);
			auto type = tracing::to_type<typename decltype(returnValue)::basic_type>();
			tracing::traceReturnOperation(type, returnValue.state);
		}
	};
	return traceFunc;
}

template <typename R, typename... FunctionArguments>
std::function<void()> createFunctionWrapper(R (*fnptr)(FunctionArguments...)) {
	return createFunctionWrapper(std::make_index_sequence<sizeof...(FunctionArguments)> {}, fnptr);
}
}} // namespace nautilus::details