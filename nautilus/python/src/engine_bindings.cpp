#include "nautilus/Engine.hpp"
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace nautilus;

/// Helper macro to define a register function for a specific signature.
/// Each Python-visible registration function captures a py::function,
/// wraps it in a C++ std::function with the correct val<T> signature,
/// and passes it to NautilusEngine::registerFunction.
#define DEFINE_REGISTER_FUNC(NAME, RET_TYPE, ...)                                                                      \
	m.def(NAME, [](const engine::NautilusEngine& eng, py::function py_func) {                                          \
		std::function<val<RET_TYPE>(__VA_ARGS__)> cpp_func = [py_func](__VA_ARGS__ args) -> val<RET_TYPE> {            \
			py::gil_scoped_acquire gil;                                                                                \
			py::object result = py_func(args);                                                                         \
			return result.cast<val<RET_TYPE>>();                                                                       \
		};                                                                                                             \
		return eng.registerFunction(cpp_func);                                                                         \
	});

void bind_engine(py::module_& m) {
	// --- Options ---
	py::class_<engine::Options>(m, "Options")
	    .def(py::init<>())
	    .def(
	        "set_string",
	        [](engine::Options& self, const std::string& key, const std::string& val) { self.setOption(key, val); },
	        py::arg("key"), py::arg("value"))
	    .def(
	        "set_bool", [](engine::Options& self, const std::string& key, bool val) { self.setOption(key, val); },
	        py::arg("key"), py::arg("value"))
	    .def(
	        "set_int", [](engine::Options& self, const std::string& key, int val) { self.setOption(key, val); },
	        py::arg("key"), py::arg("value"));

	// --- NautilusEngine ---
	py::class_<engine::NautilusEngine>(m, "NautilusEngine")
	    .def(py::init([](const std::string& backend) {
		         engine::Options opts;
		         opts.setOption("engine.backend", backend);
		         return engine::NautilusEngine(opts);
	         }),
	         py::arg("backend") = "mlir")
	    .def(py::init<engine::Options>(), py::arg("options"))
	    .def("is_compiled", &engine::NautilusEngine::isCompiled)
	    .def("backend_name", &engine::NautilusEngine::getNameOfBackend);

	// --- CallableFunction bindings for supported signatures ---
	// Each CallableFunction<R, Args...> is exposed as a Python callable.

	// (int32_t) -> int32_t
	using CF_i32_i32 = engine::CallableFunction<val<int32_t>, int32_t>;
	py::class_<CF_i32_i32>(m, "_CF_i32_i32").def("__call__", [](CF_i32_i32& self, int32_t a) { return self(a); });

	m.def("_register_i32_to_i32", [](const engine::NautilusEngine& eng, py::function py_func) {
		std::function<val<int32_t>(val<int32_t>)> cpp_func = [py_func](val<int32_t> a) -> val<int32_t> {
			py::gil_scoped_acquire gil;
			py::object result = py_func(a);
			return result.cast<val<int32_t>>();
		};
		return eng.registerFunction(cpp_func);
	});

	// (int32_t, int32_t) -> int32_t
	using CF_i32_i32_i32 = engine::CallableFunction<val<int32_t>, int32_t, int32_t>;
	py::class_<CF_i32_i32_i32>(m, "_CF_i32_i32_i32").def("__call__", [](CF_i32_i32_i32& self, int32_t a, int32_t b) {
		return self(a, b);
	});

	m.def("_register_i32_i32_to_i32", [](const engine::NautilusEngine& eng, py::function py_func) {
		std::function<val<int32_t>(val<int32_t>, val<int32_t>)> cpp_func = [py_func](val<int32_t> a,
		                                                                             val<int32_t> b) -> val<int32_t> {
			py::gil_scoped_acquire gil;
			py::object result = py_func(a, b);
			return result.cast<val<int32_t>>();
		};
		return eng.registerFunction(cpp_func);
	});

	// (int64_t) -> int64_t
	using CF_i64_i64 = engine::CallableFunction<val<int64_t>, int64_t>;
	py::class_<CF_i64_i64>(m, "_CF_i64_i64").def("__call__", [](CF_i64_i64& self, int64_t a) { return self(a); });

	m.def("_register_i64_to_i64", [](const engine::NautilusEngine& eng, py::function py_func) {
		std::function<val<int64_t>(val<int64_t>)> cpp_func = [py_func](val<int64_t> a) -> val<int64_t> {
			py::gil_scoped_acquire gil;
			py::object result = py_func(a);
			return result.cast<val<int64_t>>();
		};
		return eng.registerFunction(cpp_func);
	});

	// (int64_t, int64_t) -> int64_t
	using CF_i64_i64_i64 = engine::CallableFunction<val<int64_t>, int64_t, int64_t>;
	py::class_<CF_i64_i64_i64>(m, "_CF_i64_i64_i64").def("__call__", [](CF_i64_i64_i64& self, int64_t a, int64_t b) {
		return self(a, b);
	});

	m.def("_register_i64_i64_to_i64", [](const engine::NautilusEngine& eng, py::function py_func) {
		std::function<val<int64_t>(val<int64_t>, val<int64_t>)> cpp_func = [py_func](val<int64_t> a,
		                                                                             val<int64_t> b) -> val<int64_t> {
			py::gil_scoped_acquire gil;
			py::object result = py_func(a, b);
			return result.cast<val<int64_t>>();
		};
		return eng.registerFunction(cpp_func);
	});

	// (double) -> double
	using CF_f64_f64 = engine::CallableFunction<val<double>, double>;
	py::class_<CF_f64_f64>(m, "_CF_f64_f64").def("__call__", [](CF_f64_f64& self, double a) { return self(a); });

	m.def("_register_f64_to_f64", [](const engine::NautilusEngine& eng, py::function py_func) {
		std::function<val<double>(val<double>)> cpp_func = [py_func](val<double> a) -> val<double> {
			py::gil_scoped_acquire gil;
			py::object result = py_func(a);
			return result.cast<val<double>>();
		};
		return eng.registerFunction(cpp_func);
	});

	// (double, double) -> double
	using CF_f64_f64_f64 = engine::CallableFunction<val<double>, double, double>;
	py::class_<CF_f64_f64_f64>(m, "_CF_f64_f64_f64").def("__call__", [](CF_f64_f64_f64& self, double a, double b) {
		return self(a, b);
	});

	m.def("_register_f64_f64_to_f64", [](const engine::NautilusEngine& eng, py::function py_func) {
		std::function<val<double>(val<double>, val<double>)> cpp_func = [py_func](val<double> a,
		                                                                          val<double> b) -> val<double> {
			py::gil_scoped_acquire gil;
			py::object result = py_func(a, b);
			return result.cast<val<double>>();
		};
		return eng.registerFunction(cpp_func);
	});

	// (float) -> float
	using CF_f32_f32 = engine::CallableFunction<val<float>, float>;
	py::class_<CF_f32_f32>(m, "_CF_f32_f32").def("__call__", [](CF_f32_f32& self, float a) { return self(a); });

	m.def("_register_f32_to_f32", [](const engine::NautilusEngine& eng, py::function py_func) {
		std::function<val<float>(val<float>)> cpp_func = [py_func](val<float> a) -> val<float> {
			py::gil_scoped_acquire gil;
			py::object result = py_func(a);
			return result.cast<val<float>>();
		};
		return eng.registerFunction(cpp_func);
	});

	// (bool) -> bool
	using CF_b_b = engine::CallableFunction<val<bool>, bool>;
	py::class_<CF_b_b>(m, "_CF_b_b").def("__call__", [](CF_b_b& self, bool a) { return self(a); });

	m.def("_register_b_to_b", [](const engine::NautilusEngine& eng, py::function py_func) {
		std::function<val<bool>(val<bool>)> cpp_func = [py_func](val<bool> a) -> val<bool> {
			py::gil_scoped_acquire gil;
			py::object result = py_func(a);
			return result.cast<val<bool>>();
		};
		return eng.registerFunction(cpp_func);
	});

	// (int32_t) -> bool  (predicate)
	using CF_b_i32 = engine::CallableFunction<val<bool>, int32_t>;
	py::class_<CF_b_i32>(m, "_CF_b_i32").def("__call__", [](CF_b_i32& self, int32_t a) { return self(a); });

	m.def("_register_i32_to_b", [](const engine::NautilusEngine& eng, py::function py_func) {
		std::function<val<bool>(val<int32_t>)> cpp_func = [py_func](val<int32_t> a) -> val<bool> {
			py::gil_scoped_acquire gil;
			py::object result = py_func(a);
			return result.cast<val<bool>>();
		};
		return eng.registerFunction(cpp_func);
	});
}
