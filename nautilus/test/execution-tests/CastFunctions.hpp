#pragma once

#include <nautilus/Engine.hpp>

namespace nautilus::engine {

template<typename In, typename Out>
val<Out> implicitCastExpression(val<In> x) {
	val<Out> y = x;
	return y;
}

template<typename In, typename Out>
val<Out> staticCastExpression(val<In> x) {
	val<Out> y = static_cast<val<Out>>(x);
	return y;
}


} // namespace nautilus::engine
