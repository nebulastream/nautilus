#include "nautilus/Engine.hpp"
#include "nautilus/Module.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/tracing/symbolic_execution/TraceTerminationException.hpp"
#include "py_object_helpers.hpp"
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
[[noreturn]] inline void rethrowIfTraceTermination(py::error_already_set& e) {
	if (e.matches(getTraceTerminationExceptionType())) {
		e.restore();
		PyErr_Clear();
		throw nautilus::tracing::TraceTerminationException();
	}
	throw;
}

// ── Helper: create a C++ std::function from a Python callable ────────────────
// Wraps a py::function with GIL acquire and TraceTerminationException handling.

template <typename Ret, typename... Args>
static std::function<val<Ret>(val<Args>...)> makeCppFunc(py::function py_func) {
	return [py_func](val<Args>... args) -> val<Ret> {
		py::gil_scoped_acquire gil;
		try {
			return py_func(args...).template cast<val<Ret>>();
		} catch (py::error_already_set& e) {
			rethrowIfTraceTermination(e);
		}
	};
}

/// RAII guard that sets the external position hint for the duration of a scope.
struct PositionHintGuard {
	PositionHintGuard() {
		tracing::externalPositionHintFn = &getPythonBytecodeOffset;
	}
	~PositionHintGuard() {
		tracing::externalPositionHintFn = nullptr;
	}
};

// ── NautilusFunction wrapper ──────────────────────────────────────────────
// NautilusFunction<F> is non-copyable/non-movable and requires stable address.
// We heap-allocate one per Python function and store it in a shared_ptr.
// PyNautilusFunc<Ret, Args...> holds the shared_ptr and delegates calls.

template <typename Ret, typename... Args>
struct PyNautilusFunc {
	using NF = NautilusFunction<std::function<val<Ret>(val<Args>...)>>;
	std::shared_ptr<NF> nf;
	val<Ret> call(val<Args>... args) {
		return (*nf)(args...);
	}
};

void bind_engine(py::module_& m) {
	// ── Options ────────────────────────────────────────────────────────────
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
	        py::arg("key"), py::arg("value"))
	    .def(
	        "set_double", [](engine::Options& self, const std::string& key, double val) { self.setOption(key, val); },
	        py::arg("key"), py::arg("value"))
	    .def("set", [](engine::Options& self, const std::string& key, py::object value) {
		    // Auto-detect type: bool must be checked before int (bool is a subclass of int in Python)
		    if (py::isinstance<py::bool_>(value)) {
			    self.setOption(key, value.cast<bool>());
		    } else if (py::isinstance<py::int_>(value)) {
			    self.setOption(key, value.cast<int>());
		    } else if (py::isinstance<py::float_>(value)) {
			    self.setOption(key, value.cast<double>());
		    } else if (py::isinstance<py::str>(value)) {
			    self.setOption(key, value.cast<std::string>());
		    } else {
			    throw py::type_error("Option value must be bool, int, float, or str");
		    }
	    });

	// ── NautilusEngine ────────────────────────────────────────────────────
	py::class_<engine::NautilusEngine>(m, "NautilusEngine")
	    .def(py::init([](const std::string& backend) {
		         engine::Options opts;
		         opts.setOption("engine.backend", backend);
		         return engine::NautilusEngine(opts);
	         }),
	         py::arg("backend") = "mlir")
	    .def(py::init<engine::Options>(), py::arg("options"))
	    .def("is_compiled", &engine::NautilusEngine::isCompiled)
	    .def("backend_name", &engine::NautilusEngine::getNameOfBackend)
	    .def("create_module", &engine::NautilusEngine::createModule);

	// ── CallableFunction bindings for supported signatures ─────────────────

	// (int32_t) -> int32_t
	using CF_i32_i32 = engine::CallableFunction<val<int32_t>, int32_t>;
	py::class_<CF_i32_i32>(m, "_CF_i32_i32").def("__call__", [](CF_i32_i32& self, int32_t a) { return self(a); });

	m.def("_register_i32_to_i32", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc<int32_t, int32_t>(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// (int32_t, int32_t) -> int32_t
	using CF_i32_i32_i32 = engine::CallableFunction<val<int32_t>, int32_t, int32_t>;
	py::class_<CF_i32_i32_i32>(m, "_CF_i32_i32_i32").def("__call__", [](CF_i32_i32_i32& self, int32_t a, int32_t b) {
		return self(a, b);
	});

	m.def("_register_i32_i32_to_i32", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc<int32_t, int32_t, int32_t>(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// (int64_t) -> int64_t
	using CF_i64_i64 = engine::CallableFunction<val<int64_t>, int64_t>;
	py::class_<CF_i64_i64>(m, "_CF_i64_i64").def("__call__", [](CF_i64_i64& self, int64_t a) { return self(a); });

	m.def("_register_i64_to_i64", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc<int64_t, int64_t>(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// (int64_t, int64_t) -> int64_t
	using CF_i64_i64_i64 = engine::CallableFunction<val<int64_t>, int64_t, int64_t>;
	py::class_<CF_i64_i64_i64>(m, "_CF_i64_i64_i64").def("__call__", [](CF_i64_i64_i64& self, int64_t a, int64_t b) {
		return self(a, b);
	});

	m.def("_register_i64_i64_to_i64", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc<int64_t, int64_t, int64_t>(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// (double) -> double
	using CF_f64_f64 = engine::CallableFunction<val<double>, double>;
	py::class_<CF_f64_f64>(m, "_CF_f64_f64").def("__call__", [](CF_f64_f64& self, double a) { return self(a); });

	m.def("_register_f64_to_f64", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc<double, double>(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// (double, double) -> double
	using CF_f64_f64_f64 = engine::CallableFunction<val<double>, double, double>;
	py::class_<CF_f64_f64_f64>(m, "_CF_f64_f64_f64").def("__call__", [](CF_f64_f64_f64& self, double a, double b) {
		return self(a, b);
	});

	m.def("_register_f64_f64_to_f64", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc<double, double, double>(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// (float) -> float
	using CF_f32_f32 = engine::CallableFunction<val<float>, float>;
	py::class_<CF_f32_f32>(m, "_CF_f32_f32").def("__call__", [](CF_f32_f32& self, float a) { return self(a); });

	m.def("_register_f32_to_f32", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc<float, float>(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// (float, float) -> float
	using CF_f32_f32_f32 = engine::CallableFunction<val<float>, float, float>;
	py::class_<CF_f32_f32_f32>(m, "_CF_f32_f32_f32").def("__call__", [](CF_f32_f32_f32& self, float a, float b) {
		return self(a, b);
	});

	m.def("_register_f32_f32_to_f32", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc<float, float, float>(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// (bool) -> bool
	using CF_b_b = engine::CallableFunction<val<bool>, bool>;
	py::class_<CF_b_b>(m, "_CF_b_b").def("__call__", [](CF_b_b& self, bool a) { return self(a); });

	m.def("_register_b_to_b", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc<bool, bool>(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// (int32_t) -> bool  (predicate)
	using CF_b_i32 = engine::CallableFunction<val<bool>, int32_t>;
	py::class_<CF_b_i32>(m, "_CF_b_i32").def("__call__", [](CF_b_i32& self, int32_t a) { return self(a); });

	m.def("_register_i32_to_b", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc<bool, int32_t>(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// (void*) -> void*  (generic object, unary)
	using CF_obj_obj = engine::CallableFunction<val<void*>, void*>;
	py::class_<CF_obj_obj>(m, "_CF_obj_obj").def("__call__", [](CF_obj_obj& self, py::object a) {
		arenaClear();
		void* result = self(static_cast<void*>(a.ptr()));
		Py_XINCREF(static_cast<PyObject*>(result));
		arenaClear();
		return py::reinterpret_steal<py::object>(static_cast<PyObject*>(result));
	});

	m.def("_register_obj_to_obj", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc<void*, void*>(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// (void*, void*) -> void*  (generic object, binary)
	using CF_obj_obj_obj = engine::CallableFunction<val<void*>, void*, void*>;
	py::class_<CF_obj_obj_obj>(m, "_CF_obj_obj_obj")
	    .def("__call__", [](CF_obj_obj_obj& self, py::object a, py::object b) {
		    arenaClear();
		    void* result = self(static_cast<void*>(a.ptr()), static_cast<void*>(b.ptr()));
		    Py_XINCREF(static_cast<PyObject*>(result));
		    arenaClear();
		    return py::reinterpret_steal<py::object>(static_cast<PyObject*>(result));
	    });

	m.def("_register_obj_obj_to_obj", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc<void*, void*, void*>(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// ── 3-argument signatures ─────────────────────────────────────────────

	// (int32_t, int32_t, int32_t) -> int32_t
	using CF_i32_i32_i32_i32 = engine::CallableFunction<val<int32_t>, int32_t, int32_t, int32_t>;
	py::class_<CF_i32_i32_i32_i32>(m, "_CF_i32_i32_i32_i32")
	    .def("__call__", [](CF_i32_i32_i32_i32& self, int32_t a, int32_t b, int32_t c) { return self(a, b, c); });

	m.def("_register_i32_i32_i32_to_i32", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc<int32_t, int32_t, int32_t, int32_t>(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// (int64_t, int64_t, int64_t) -> int64_t
	using CF_i64_i64_i64_i64 = engine::CallableFunction<val<int64_t>, int64_t, int64_t, int64_t>;
	py::class_<CF_i64_i64_i64_i64>(m, "_CF_i64_i64_i64_i64")
	    .def("__call__", [](CF_i64_i64_i64_i64& self, int64_t a, int64_t b, int64_t c) { return self(a, b, c); });

	m.def("_register_i64_i64_i64_to_i64", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc<int64_t, int64_t, int64_t, int64_t>(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// (double, double, double) -> double
	using CF_f64_f64_f64_f64 = engine::CallableFunction<val<double>, double, double, double>;
	py::class_<CF_f64_f64_f64_f64>(m, "_CF_f64_f64_f64_f64")
	    .def("__call__", [](CF_f64_f64_f64_f64& self, double a, double b, double c) { return self(a, b, c); });

	m.def("_register_f64_f64_f64_to_f64", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc<double, double, double, double>(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// (float, float, float) -> float
	using CF_f32_f32_f32_f32 = engine::CallableFunction<val<float>, float, float, float>;
	py::class_<CF_f32_f32_f32_f32>(m, "_CF_f32_f32_f32_f32")
	    .def("__call__", [](CF_f32_f32_f32_f32& self, float a, float b, float c) { return self(a, b, c); });

	m.def("_register_f32_f32_f32_to_f32", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc<float, float, float, float>(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// (void*, void*, void*) -> void*  (generic object, ternary)
	using CF_obj_obj_obj_obj = engine::CallableFunction<val<void*>, void*, void*, void*>;
	py::class_<CF_obj_obj_obj_obj>(m, "_CF_obj_obj_obj_obj")
	    .def("__call__", [](CF_obj_obj_obj_obj& self, py::object a, py::object b, py::object c) {
		    arenaClear();
		    void* result = self(static_cast<void*>(a.ptr()), static_cast<void*>(b.ptr()), static_cast<void*>(c.ptr()));
		    Py_XINCREF(static_cast<PyObject*>(result));
		    arenaClear();
		    return py::reinterpret_steal<py::object>(static_cast<PyObject*>(result));
	    });

	m.def("_register_obj_obj_obj_to_obj", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc<void*, void*, void*, void*>(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// ── NautilusModule ────────────────────────────────────────────────────
	py::class_<engine::NautilusModule>(m, "_NautilusModule")
	    .def("register_i32_to_i32",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc<int32_t, int32_t>(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("register_i32_i32_to_i32",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc<int32_t, int32_t, int32_t>(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("register_i64_to_i64",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc<int64_t, int64_t>(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("register_i64_i64_to_i64",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc<int64_t, int64_t, int64_t>(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("register_f64_to_f64",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc<double, double>(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("register_f64_f64_to_f64",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc<double, double, double>(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("register_f32_to_f32",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc<float, float>(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("register_f32_f32_to_f32",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc<float, float, float>(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("register_b_to_b",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc<bool, bool>(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("register_i32_to_b",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc<bool, int32_t>(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("register_obj_to_obj",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc<void*, void*>(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("register_obj_obj_to_obj",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc<void*, void*, void*>(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    // 3-argument register methods
	    .def("register_i32_i32_i32_to_i32",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc<int32_t, int32_t, int32_t, int32_t>(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("register_i64_i64_i64_to_i64",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc<int64_t, int64_t, int64_t, int64_t>(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("register_f64_f64_f64_to_f64",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc<double, double, double, double>(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("register_f32_f32_f32_to_f32",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc<float, float, float, float>(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("register_obj_obj_obj_to_obj",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc<void*, void*, void*, void*>(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("compile", [](engine::NautilusModule& self) {
		    PositionHintGuard guard;
		    return self.compile();
	    });

	// ── CompiledModule ────────────────────────────────────────────────────
	py::class_<engine::CompiledModule>(m, "_CompiledModule")
	    .def("get_i32_i32", [](engine::CompiledModule& self,
	                           const std::string& name) { return self.getFunction<int32_t(int32_t)>(name); })
	    .def("get_i32_i32_i32",
	         [](engine::CompiledModule& self, const std::string& name) {
		         return self.getFunction<int32_t(int32_t, int32_t)>(name);
	         })
	    .def("get_i64_i64", [](engine::CompiledModule& self,
	                           const std::string& name) { return self.getFunction<int64_t(int64_t)>(name); })
	    .def("get_i64_i64_i64",
	         [](engine::CompiledModule& self, const std::string& name) {
		         return self.getFunction<int64_t(int64_t, int64_t)>(name);
	         })
	    .def("get_f64_f64", [](engine::CompiledModule& self,
	                           const std::string& name) { return self.getFunction<double(double)>(name); })
	    .def("get_f64_f64_f64", [](engine::CompiledModule& self,
	                               const std::string& name) { return self.getFunction<double(double, double)>(name); })
	    .def("get_f32_f32",
	         [](engine::CompiledModule& self, const std::string& name) { return self.getFunction<float(float)>(name); })
	    .def("get_f32_f32_f32", [](engine::CompiledModule& self,
	                               const std::string& name) { return self.getFunction<float(float, float)>(name); })
	    .def("get_b_b",
	         [](engine::CompiledModule& self, const std::string& name) { return self.getFunction<bool(bool)>(name); })
	    .def("get_b_i32", [](engine::CompiledModule& self,
	                         const std::string& name) { return self.getFunction<bool(int32_t)>(name); })
	    .def("get_obj_obj",
	         [](engine::CompiledModule& self, const std::string& name) { return self.getFunction<void*(void*)>(name); })
	    .def("get_obj_obj_obj", [](engine::CompiledModule& self,
	                               const std::string& name) { return self.getFunction<void*(void*, void*)>(name); })
	    // 3-argument get methods
	    .def("get_i32_i32_i32_i32",
	         [](engine::CompiledModule& self, const std::string& name) {
		         return self.getFunction<int32_t(int32_t, int32_t, int32_t)>(name);
	         })
	    .def("get_i64_i64_i64_i64",
	         [](engine::CompiledModule& self, const std::string& name) {
		         return self.getFunction<int64_t(int64_t, int64_t, int64_t)>(name);
	         })
	    .def("get_f64_f64_f64_f64",
	         [](engine::CompiledModule& self, const std::string& name) {
		         return self.getFunction<double(double, double, double)>(name);
	         })
	    .def("get_f32_f32_f32_f32",
	         [](engine::CompiledModule& self, const std::string& name) {
		         return self.getFunction<float(float, float, float)>(name);
	         })
	    .def("get_obj_obj_obj_obj", [](engine::CompiledModule& self, const std::string& name) {
		    return self.getFunction<void*(void*, void*, void*)>(name);
	    });

	// ── ModuleFunction bindings ───────────────────────────────────────────
	py::class_<engine::ModuleFunction<int32_t(int32_t)>>(m, "_MF_i32_i32")
	    .def("__call__", [](engine::ModuleFunction<int32_t(int32_t)>& self, int32_t a) { return self(a); });

	py::class_<engine::ModuleFunction<int32_t(int32_t, int32_t)>>(m, "_MF_i32_i32_i32")
	    .def("__call__",
	         [](engine::ModuleFunction<int32_t(int32_t, int32_t)>& self, int32_t a, int32_t b) { return self(a, b); });

	py::class_<engine::ModuleFunction<int64_t(int64_t)>>(m, "_MF_i64_i64")
	    .def("__call__", [](engine::ModuleFunction<int64_t(int64_t)>& self, int64_t a) { return self(a); });

	py::class_<engine::ModuleFunction<int64_t(int64_t, int64_t)>>(m, "_MF_i64_i64_i64")
	    .def("__call__",
	         [](engine::ModuleFunction<int64_t(int64_t, int64_t)>& self, int64_t a, int64_t b) { return self(a, b); });

	py::class_<engine::ModuleFunction<double(double)>>(m, "_MF_f64_f64")
	    .def("__call__", [](engine::ModuleFunction<double(double)>& self, double a) { return self(a); });

	py::class_<engine::ModuleFunction<double(double, double)>>(m, "_MF_f64_f64_f64")
	    .def("__call__",
	         [](engine::ModuleFunction<double(double, double)>& self, double a, double b) { return self(a, b); });

	py::class_<engine::ModuleFunction<float(float)>>(m, "_MF_f32_f32")
	    .def("__call__", [](engine::ModuleFunction<float(float)>& self, float a) { return self(a); });

	py::class_<engine::ModuleFunction<float(float, float)>>(m, "_MF_f32_f32_f32")
	    .def("__call__",
	         [](engine::ModuleFunction<float(float, float)>& self, float a, float b) { return self(a, b); });

	py::class_<engine::ModuleFunction<bool(bool)>>(m, "_MF_b_b")
	    .def("__call__", [](engine::ModuleFunction<bool(bool)>& self, bool a) { return self(a); });

	py::class_<engine::ModuleFunction<bool(int32_t)>>(m, "_MF_b_i32")
	    .def("__call__", [](engine::ModuleFunction<bool(int32_t)>& self, int32_t a) { return self(a); });

	py::class_<engine::ModuleFunction<void*(void*)>>(m, "_MF_obj_obj")
	    .def("__call__", [](engine::ModuleFunction<void*(void*)>& self, py::object a) {
		    arenaClear();
		    void* result = self(static_cast<void*>(a.ptr()));
		    Py_XINCREF(static_cast<PyObject*>(result));
		    arenaClear();
		    return py::reinterpret_steal<py::object>(static_cast<PyObject*>(result));
	    });

	py::class_<engine::ModuleFunction<void*(void*, void*)>>(m, "_MF_obj_obj_obj")
	    .def("__call__", [](engine::ModuleFunction<void*(void*, void*)>& self, py::object a, py::object b) {
		    arenaClear();
		    void* result = self(static_cast<void*>(a.ptr()), static_cast<void*>(b.ptr()));
		    Py_XINCREF(static_cast<PyObject*>(result));
		    arenaClear();
		    return py::reinterpret_steal<py::object>(static_cast<PyObject*>(result));
	    });

	// 3-argument ModuleFunction bindings
	py::class_<engine::ModuleFunction<int32_t(int32_t, int32_t, int32_t)>>(m, "_MF_i32_i32_i32_i32")
	    .def("__call__", [](engine::ModuleFunction<int32_t(int32_t, int32_t, int32_t)>& self, int32_t a, int32_t b,
	                        int32_t c) { return self(a, b, c); });

	py::class_<engine::ModuleFunction<int64_t(int64_t, int64_t, int64_t)>>(m, "_MF_i64_i64_i64_i64")
	    .def("__call__", [](engine::ModuleFunction<int64_t(int64_t, int64_t, int64_t)>& self, int64_t a, int64_t b,
	                        int64_t c) { return self(a, b, c); });

	py::class_<engine::ModuleFunction<double(double, double, double)>>(m, "_MF_f64_f64_f64_f64")
	    .def("__call__", [](engine::ModuleFunction<double(double, double, double)>& self, double a, double b,
	                        double c) { return self(a, b, c); });

	py::class_<engine::ModuleFunction<float(float, float, float)>>(m, "_MF_f32_f32_f32_f32")
	    .def("__call__", [](engine::ModuleFunction<float(float, float, float)>& self, float a, float b, float c) {
		    return self(a, b, c);
	    });

	py::class_<engine::ModuleFunction<void*(void*, void*, void*)>>(m, "_MF_obj_obj_obj_obj")
	    .def("__call__", [](engine::ModuleFunction<void*(void*, void*, void*)>& self, py::object a, py::object b,
	                        py::object c) {
		    arenaClear();
		    void* result = self(static_cast<void*>(a.ptr()), static_cast<void*>(b.ptr()), static_cast<void*>(c.ptr()));
		    Py_XINCREF(static_cast<PyObject*>(result));
		    arenaClear();
		    return py::reinterpret_steal<py::object>(static_cast<PyObject*>(result));
	    });

	// ── NautilusFunction wrappers ─────────────────────────────────────────
	// Helper to register a PyNautilusFunc<Ret, Args...> and its factory.
	// Each instantiation gets a unique pybind11 class with a string name.

#define REGISTER_NAUTILUS_FUNC(LABEL, RET, ...)                                                                        \
	using PNF_##LABEL = PyNautilusFunc<RET, __VA_ARGS__>;                                                              \
	py::class_<PNF_##LABEL>(m, "_NautilusFunc_" #LABEL).def("__call__", &PNF_##LABEL::call);                           \
	m.def("_create_nautilus_func_" #LABEL, [](const std::string& name, py::function py_func) {                         \
		auto cpp_func = makeCppFunc<RET, __VA_ARGS__>(py_func);                                                        \
		auto nf = std::make_shared<PNF_##LABEL::NF>(name, std::move(cpp_func));                                        \
		return PNF_##LABEL {std::move(nf)};                                                                            \
	})

	REGISTER_NAUTILUS_FUNC(i32_i32, int32_t, int32_t);
	REGISTER_NAUTILUS_FUNC(i32_i32_i32, int32_t, int32_t, int32_t);
	REGISTER_NAUTILUS_FUNC(i64_i64, int64_t, int64_t);
	REGISTER_NAUTILUS_FUNC(i64_i64_i64, int64_t, int64_t, int64_t);
	REGISTER_NAUTILUS_FUNC(f64_f64, double, double);
	REGISTER_NAUTILUS_FUNC(f64_f64_f64, double, double, double);
	REGISTER_NAUTILUS_FUNC(f32_f32, float, float);
	REGISTER_NAUTILUS_FUNC(f32_f32_f32, float, float, float);
	REGISTER_NAUTILUS_FUNC(b_b, bool, bool);
	REGISTER_NAUTILUS_FUNC(b_i32, bool, int32_t);
	REGISTER_NAUTILUS_FUNC(obj_obj, void*, void*);
	REGISTER_NAUTILUS_FUNC(obj_obj_obj, void*, void*, void*);
	// 3-argument NautilusFunction wrappers
	REGISTER_NAUTILUS_FUNC(i32_i32_i32_i32, int32_t, int32_t, int32_t, int32_t);
	REGISTER_NAUTILUS_FUNC(i64_i64_i64_i64, int64_t, int64_t, int64_t, int64_t);
	REGISTER_NAUTILUS_FUNC(f64_f64_f64_f64, double, double, double, double);
	REGISTER_NAUTILUS_FUNC(f32_f32_f32_f32, float, float, float, float);
	REGISTER_NAUTILUS_FUNC(obj_obj_obj_obj, void*, void*, void*, void*);

#undef REGISTER_NAUTILUS_FUNC
}
