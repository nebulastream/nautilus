#include "nautilus/Engine.hpp"
#include "nautilus/tracing/symbolic_execution/TraceTerminationException.hpp"
#include <Python.h>
#include <frameobject.h>
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

// External position hint for disambiguating Python call sites that share the
// same C++ stack trace (e.g. two different __gt__ comparisons in user code).
#include "nautilus/tracing/ExternalPositionHint.hpp"

namespace py = pybind11;
using namespace nautilus;

// Defined in nautilus_module.cpp — the Python exception type for TraceTerminationException.
PyObject* getTraceTerminationExceptionType();

/// Returns the Python bytecode offset of the current frame.
/// Different Python source positions produce different offsets, which
/// disambiguates operations that route through the same pybind11 dispatcher.
static uint64_t getPythonBytecodeOffset() {
	PyFrameObject* frame = PyEval_GetFrame();
	if (!frame) {
		return 0;
	}
#if PY_VERSION_HEX >= 0x030b0000 // Python 3.11+
	return static_cast<uint64_t>(static_cast<uint32_t>(PyFrame_GetLasti(frame)));
#else
	return static_cast<uint64_t>(static_cast<uint32_t>(frame->f_lasti));
#endif
}

/// Re-throws TraceTerminationException if the Python error is our registered type.
/// Otherwise re-throws the original py::error_already_set.
/// This is critical for tracing: the C++ trace loop catches TraceTerminationException
/// to handle control-flow merges and loop detection. Without this, the exception
/// gets swallowed by pybind11's generic exception translation.
[[noreturn]] inline void rethrowIfTraceTermination(py::error_already_set& e) {
	if (e.matches(getTraceTerminationExceptionType())) {
		e.restore();
		PyErr_Clear();
		throw nautilus::tracing::TraceTerminationException();
	}
	throw;
}

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
			try {
				py::object result = py_func(a);
				return result.cast<val<int32_t>>();
			} catch (py::error_already_set& e) {
				rethrowIfTraceTermination(e);
			}
		};
		tracing::externalPositionHintFn = &getPythonBytecodeOffset;
		auto result = eng.registerFunction(cpp_func);
		tracing::externalPositionHintFn = nullptr;
		return result;
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
			try {
				py::object result = py_func(a, b);
				return result.cast<val<int32_t>>();
			} catch (py::error_already_set& e) {
				rethrowIfTraceTermination(e);
			}
		};
		tracing::externalPositionHintFn = &getPythonBytecodeOffset;
		auto result = eng.registerFunction(cpp_func);
		tracing::externalPositionHintFn = nullptr;
		return result;
	});

	// (int64_t) -> int64_t
	using CF_i64_i64 = engine::CallableFunction<val<int64_t>, int64_t>;
	py::class_<CF_i64_i64>(m, "_CF_i64_i64").def("__call__", [](CF_i64_i64& self, int64_t a) { return self(a); });

	m.def("_register_i64_to_i64", [](const engine::NautilusEngine& eng, py::function py_func) {
		std::function<val<int64_t>(val<int64_t>)> cpp_func = [py_func](val<int64_t> a) -> val<int64_t> {
			py::gil_scoped_acquire gil;
			try {
				py::object result = py_func(a);
				return result.cast<val<int64_t>>();
			} catch (py::error_already_set& e) {
				rethrowIfTraceTermination(e);
			}
		};
		tracing::externalPositionHintFn = &getPythonBytecodeOffset;
		auto result = eng.registerFunction(cpp_func);
		tracing::externalPositionHintFn = nullptr;
		return result;
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
			try {
				py::object result = py_func(a, b);
				return result.cast<val<int64_t>>();
			} catch (py::error_already_set& e) {
				rethrowIfTraceTermination(e);
			}
		};
		tracing::externalPositionHintFn = &getPythonBytecodeOffset;
		auto result = eng.registerFunction(cpp_func);
		tracing::externalPositionHintFn = nullptr;
		return result;
	});

	// (double) -> double
	using CF_f64_f64 = engine::CallableFunction<val<double>, double>;
	py::class_<CF_f64_f64>(m, "_CF_f64_f64").def("__call__", [](CF_f64_f64& self, double a) { return self(a); });

	m.def("_register_f64_to_f64", [](const engine::NautilusEngine& eng, py::function py_func) {
		std::function<val<double>(val<double>)> cpp_func = [py_func](val<double> a) -> val<double> {
			py::gil_scoped_acquire gil;
			try {
				py::object result = py_func(a);
				return result.cast<val<double>>();
			} catch (py::error_already_set& e) {
				rethrowIfTraceTermination(e);
			}
		};
		tracing::externalPositionHintFn = &getPythonBytecodeOffset;
		auto result = eng.registerFunction(cpp_func);
		tracing::externalPositionHintFn = nullptr;
		return result;
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
			try {
				py::object result = py_func(a, b);
				return result.cast<val<double>>();
			} catch (py::error_already_set& e) {
				rethrowIfTraceTermination(e);
			}
		};
		tracing::externalPositionHintFn = &getPythonBytecodeOffset;
		auto result = eng.registerFunction(cpp_func);
		tracing::externalPositionHintFn = nullptr;
		return result;
	});

	// (float) -> float
	using CF_f32_f32 = engine::CallableFunction<val<float>, float>;
	py::class_<CF_f32_f32>(m, "_CF_f32_f32").def("__call__", [](CF_f32_f32& self, float a) { return self(a); });

	m.def("_register_f32_to_f32", [](const engine::NautilusEngine& eng, py::function py_func) {
		std::function<val<float>(val<float>)> cpp_func = [py_func](val<float> a) -> val<float> {
			py::gil_scoped_acquire gil;
			try {
				py::object result = py_func(a);
				return result.cast<val<float>>();
			} catch (py::error_already_set& e) {
				rethrowIfTraceTermination(e);
			}
		};
		tracing::externalPositionHintFn = &getPythonBytecodeOffset;
		auto result = eng.registerFunction(cpp_func);
		tracing::externalPositionHintFn = nullptr;
		return result;
	});

	// (bool) -> bool
	using CF_b_b = engine::CallableFunction<val<bool>, bool>;
	py::class_<CF_b_b>(m, "_CF_b_b").def("__call__", [](CF_b_b& self, bool a) { return self(a); });

	m.def("_register_b_to_b", [](const engine::NautilusEngine& eng, py::function py_func) {
		std::function<val<bool>(val<bool>)> cpp_func = [py_func](val<bool> a) -> val<bool> {
			py::gil_scoped_acquire gil;
			try {
				py::object result = py_func(a);
				return result.cast<val<bool>>();
			} catch (py::error_already_set& e) {
				rethrowIfTraceTermination(e);
			}
		};
		tracing::externalPositionHintFn = &getPythonBytecodeOffset;
		auto result = eng.registerFunction(cpp_func);
		tracing::externalPositionHintFn = nullptr;
		return result;
	});

	// (int32_t) -> bool  (predicate)
	using CF_b_i32 = engine::CallableFunction<val<bool>, int32_t>;
	py::class_<CF_b_i32>(m, "_CF_b_i32").def("__call__", [](CF_b_i32& self, int32_t a) { return self(a); });

	m.def("_register_i32_to_b", [](const engine::NautilusEngine& eng, py::function py_func) {
		std::function<val<bool>(val<int32_t>)> cpp_func = [py_func](val<int32_t> a) -> val<bool> {
			py::gil_scoped_acquire gil;
			try {
				py::object result = py_func(a);
				return result.cast<val<bool>>();
			} catch (py::error_already_set& e) {
				rethrowIfTraceTermination(e);
			}
		};
		tracing::externalPositionHintFn = &getPythonBytecodeOffset;
		auto result = eng.registerFunction(cpp_func);
		tracing::externalPositionHintFn = nullptr;
		return result;
	});
}
