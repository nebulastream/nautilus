#include "nautilus/Engine.hpp"
#include "nautilus/Module.hpp"
#include "nautilus/nautilus_function.hpp"
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
[[noreturn]] inline void rethrowIfTraceTermination(py::error_already_set& e) {
	if (e.matches(getTraceTerminationExceptionType())) {
		e.restore();
		PyErr_Clear();
		throw nautilus::tracing::TraceTerminationException();
	}
	throw;
}

// ── Helpers: create a C++ std::function from a Python callable ─────────────
// Each wraps a py::function with GIL acquire and TraceTerminationException handling.

static std::function<val<int32_t>(val<int32_t>)> makeCppFunc_i32_i32(py::function py_func) {
	return [py_func](val<int32_t> a) -> val<int32_t> {
		py::gil_scoped_acquire gil;
		try {
			return py_func(a).cast<val<int32_t>>();
		} catch (py::error_already_set& e) {
			rethrowIfTraceTermination(e);
		}
	};
}

static std::function<val<int32_t>(val<int32_t>, val<int32_t>)> makeCppFunc_i32_i32_i32(py::function py_func) {
	return [py_func](val<int32_t> a, val<int32_t> b) -> val<int32_t> {
		py::gil_scoped_acquire gil;
		try {
			return py_func(a, b).cast<val<int32_t>>();
		} catch (py::error_already_set& e) {
			rethrowIfTraceTermination(e);
		}
	};
}

static std::function<val<int64_t>(val<int64_t>)> makeCppFunc_i64_i64(py::function py_func) {
	return [py_func](val<int64_t> a) -> val<int64_t> {
		py::gil_scoped_acquire gil;
		try {
			return py_func(a).cast<val<int64_t>>();
		} catch (py::error_already_set& e) {
			rethrowIfTraceTermination(e);
		}
	};
}

static std::function<val<int64_t>(val<int64_t>, val<int64_t>)> makeCppFunc_i64_i64_i64(py::function py_func) {
	return [py_func](val<int64_t> a, val<int64_t> b) -> val<int64_t> {
		py::gil_scoped_acquire gil;
		try {
			return py_func(a, b).cast<val<int64_t>>();
		} catch (py::error_already_set& e) {
			rethrowIfTraceTermination(e);
		}
	};
}

static std::function<val<double>(val<double>)> makeCppFunc_f64_f64(py::function py_func) {
	return [py_func](val<double> a) -> val<double> {
		py::gil_scoped_acquire gil;
		try {
			return py_func(a).cast<val<double>>();
		} catch (py::error_already_set& e) {
			rethrowIfTraceTermination(e);
		}
	};
}

static std::function<val<double>(val<double>, val<double>)> makeCppFunc_f64_f64_f64(py::function py_func) {
	return [py_func](val<double> a, val<double> b) -> val<double> {
		py::gil_scoped_acquire gil;
		try {
			return py_func(a, b).cast<val<double>>();
		} catch (py::error_already_set& e) {
			rethrowIfTraceTermination(e);
		}
	};
}

static std::function<val<float>(val<float>)> makeCppFunc_f32_f32(py::function py_func) {
	return [py_func](val<float> a) -> val<float> {
		py::gil_scoped_acquire gil;
		try {
			return py_func(a).cast<val<float>>();
		} catch (py::error_already_set& e) {
			rethrowIfTraceTermination(e);
		}
	};
}

static std::function<val<float>(val<float>, val<float>)> makeCppFunc_f32_f32_f32(py::function py_func) {
	return [py_func](val<float> a, val<float> b) -> val<float> {
		py::gil_scoped_acquire gil;
		try {
			return py_func(a, b).cast<val<float>>();
		} catch (py::error_already_set& e) {
			rethrowIfTraceTermination(e);
		}
	};
}

static std::function<val<bool>(val<bool>)> makeCppFunc_b_b(py::function py_func) {
	return [py_func](val<bool> a) -> val<bool> {
		py::gil_scoped_acquire gil;
		try {
			return py_func(a).cast<val<bool>>();
		} catch (py::error_already_set& e) {
			rethrowIfTraceTermination(e);
		}
	};
}

static std::function<val<bool>(val<int32_t>)> makeCppFunc_b_i32(py::function py_func) {
	return [py_func](val<int32_t> a) -> val<bool> {
		py::gil_scoped_acquire gil;
		try {
			return py_func(a).cast<val<bool>>();
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

// ── NautilusFunction wrappers ──────────────────────────────────────────────
// NautilusFunction<F> is non-copyable/non-movable and requires stable address.
// We heap-allocate one per Python function and store it in a shared_ptr.
// The Python wrapper (PyNautilusFunc_*) holds the shared_ptr and delegates calls.

// For (val<int32_t>) -> val<int32_t>
using NF_i32_i32 = NautilusFunction<std::function<val<int32_t>(val<int32_t>)>>;
struct PyNautilusFunc_i32_i32 {
	std::shared_ptr<NF_i32_i32> nf;
	val<int32_t> call(val<int32_t> a) {
		return (*nf)(a);
	}
};

// For (val<int32_t>, val<int32_t>) -> val<int32_t>
using NF_i32_i32_i32 = NautilusFunction<std::function<val<int32_t>(val<int32_t>, val<int32_t>)>>;
struct PyNautilusFunc_i32_i32_i32 {
	std::shared_ptr<NF_i32_i32_i32> nf;
	val<int32_t> call(val<int32_t> a, val<int32_t> b) {
		return (*nf)(a, b);
	}
};

// For (val<int64_t>) -> val<int64_t>
using NF_i64_i64 = NautilusFunction<std::function<val<int64_t>(val<int64_t>)>>;
struct PyNautilusFunc_i64_i64 {
	std::shared_ptr<NF_i64_i64> nf;
	val<int64_t> call(val<int64_t> a) {
		return (*nf)(a);
	}
};

// For (val<int64_t>, val<int64_t>) -> val<int64_t>
using NF_i64_i64_i64 = NautilusFunction<std::function<val<int64_t>(val<int64_t>, val<int64_t>)>>;
struct PyNautilusFunc_i64_i64_i64 {
	std::shared_ptr<NF_i64_i64_i64> nf;
	val<int64_t> call(val<int64_t> a, val<int64_t> b) {
		return (*nf)(a, b);
	}
};

// For (val<double>) -> val<double>
using NF_f64_f64 = NautilusFunction<std::function<val<double>(val<double>)>>;
struct PyNautilusFunc_f64_f64 {
	std::shared_ptr<NF_f64_f64> nf;
	val<double> call(val<double> a) {
		return (*nf)(a);
	}
};

// For (val<double>, val<double>) -> val<double>
using NF_f64_f64_f64 = NautilusFunction<std::function<val<double>(val<double>, val<double>)>>;
struct PyNautilusFunc_f64_f64_f64 {
	std::shared_ptr<NF_f64_f64_f64> nf;
	val<double> call(val<double> a, val<double> b) {
		return (*nf)(a, b);
	}
};

// For (val<float>) -> val<float>
using NF_f32_f32 = NautilusFunction<std::function<val<float>(val<float>)>>;
struct PyNautilusFunc_f32_f32 {
	std::shared_ptr<NF_f32_f32> nf;
	val<float> call(val<float> a) {
		return (*nf)(a);
	}
};

// For (val<float>, val<float>) -> val<float>
using NF_f32_f32_f32 = NautilusFunction<std::function<val<float>(val<float>, val<float>)>>;
struct PyNautilusFunc_f32_f32_f32 {
	std::shared_ptr<NF_f32_f32_f32> nf;
	val<float> call(val<float> a, val<float> b) {
		return (*nf)(a, b);
	}
};

// For (val<bool>) -> val<bool>
using NF_b_b = NautilusFunction<std::function<val<bool>(val<bool>)>>;
struct PyNautilusFunc_b_b {
	std::shared_ptr<NF_b_b> nf;
	val<bool> call(val<bool> a) {
		return (*nf)(a);
	}
};

// For (val<int32_t>) -> val<bool>
using NF_b_i32 = NautilusFunction<std::function<val<bool>(val<int32_t>)>>;
struct PyNautilusFunc_b_i32 {
	std::shared_ptr<NF_b_i32> nf;
	val<bool> call(val<int32_t> a) {
		return (*nf)(a);
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
	        py::arg("key"), py::arg("value"));

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
		auto cpp_func = makeCppFunc_i32_i32(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// (int32_t, int32_t) -> int32_t
	using CF_i32_i32_i32 = engine::CallableFunction<val<int32_t>, int32_t, int32_t>;
	py::class_<CF_i32_i32_i32>(m, "_CF_i32_i32_i32").def("__call__", [](CF_i32_i32_i32& self, int32_t a, int32_t b) {
		return self(a, b);
	});

	m.def("_register_i32_i32_to_i32", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc_i32_i32_i32(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// (int64_t) -> int64_t
	using CF_i64_i64 = engine::CallableFunction<val<int64_t>, int64_t>;
	py::class_<CF_i64_i64>(m, "_CF_i64_i64").def("__call__", [](CF_i64_i64& self, int64_t a) { return self(a); });

	m.def("_register_i64_to_i64", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc_i64_i64(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// (int64_t, int64_t) -> int64_t
	using CF_i64_i64_i64 = engine::CallableFunction<val<int64_t>, int64_t, int64_t>;
	py::class_<CF_i64_i64_i64>(m, "_CF_i64_i64_i64").def("__call__", [](CF_i64_i64_i64& self, int64_t a, int64_t b) {
		return self(a, b);
	});

	m.def("_register_i64_i64_to_i64", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc_i64_i64_i64(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// (double) -> double
	using CF_f64_f64 = engine::CallableFunction<val<double>, double>;
	py::class_<CF_f64_f64>(m, "_CF_f64_f64").def("__call__", [](CF_f64_f64& self, double a) { return self(a); });

	m.def("_register_f64_to_f64", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc_f64_f64(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// (double, double) -> double
	using CF_f64_f64_f64 = engine::CallableFunction<val<double>, double, double>;
	py::class_<CF_f64_f64_f64>(m, "_CF_f64_f64_f64").def("__call__", [](CF_f64_f64_f64& self, double a, double b) {
		return self(a, b);
	});

	m.def("_register_f64_f64_to_f64", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc_f64_f64_f64(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// (float) -> float
	using CF_f32_f32 = engine::CallableFunction<val<float>, float>;
	py::class_<CF_f32_f32>(m, "_CF_f32_f32").def("__call__", [](CF_f32_f32& self, float a) { return self(a); });

	m.def("_register_f32_to_f32", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc_f32_f32(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// (float, float) -> float
	using CF_f32_f32_f32 = engine::CallableFunction<val<float>, float, float>;
	py::class_<CF_f32_f32_f32>(m, "_CF_f32_f32_f32").def("__call__", [](CF_f32_f32_f32& self, float a, float b) {
		return self(a, b);
	});

	m.def("_register_f32_f32_to_f32", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc_f32_f32_f32(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// (bool) -> bool
	using CF_b_b = engine::CallableFunction<val<bool>, bool>;
	py::class_<CF_b_b>(m, "_CF_b_b").def("__call__", [](CF_b_b& self, bool a) { return self(a); });

	m.def("_register_b_to_b", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc_b_b(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// (int32_t) -> bool  (predicate)
	using CF_b_i32 = engine::CallableFunction<val<bool>, int32_t>;
	py::class_<CF_b_i32>(m, "_CF_b_i32").def("__call__", [](CF_b_i32& self, int32_t a) { return self(a); });

	m.def("_register_i32_to_b", [](const engine::NautilusEngine& eng, py::function py_func) {
		auto cpp_func = makeCppFunc_b_i32(py_func);
		PositionHintGuard guard;
		return eng.registerFunction(cpp_func);
	});

	// ── NautilusModule ────────────────────────────────────────────────────
	py::class_<engine::NautilusModule>(m, "_NautilusModule")
	    .def("register_i32_to_i32",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc_i32_i32(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("register_i32_i32_to_i32",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc_i32_i32_i32(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("register_i64_to_i64",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc_i64_i64(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("register_i64_i64_to_i64",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc_i64_i64_i64(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("register_f64_to_f64",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc_f64_f64(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("register_f64_f64_to_f64",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc_f64_f64_f64(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("register_f32_to_f32",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc_f32_f32(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("register_f32_f32_to_f32",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc_f32_f32_f32(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("register_b_to_b",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc_b_b(py_func);
		         self.registerFunction(name, std::move(cpp_func));
	         })
	    .def("register_i32_to_b",
	         [](engine::NautilusModule& self, const std::string& name, py::function py_func) {
		         auto cpp_func = makeCppFunc_b_i32(py_func);
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
	    .def("get_b_i32", [](engine::CompiledModule& self, const std::string& name) {
		    return self.getFunction<bool(int32_t)>(name);
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

	// ── NautilusFunction wrappers ─────────────────────────────────────────

	py::class_<PyNautilusFunc_i32_i32>(m, "_NautilusFunc_i32_i32").def("__call__", &PyNautilusFunc_i32_i32::call);

	m.def("_create_nautilus_func_i32_i32", [](const std::string& name, py::function py_func) {
		auto cpp_func = makeCppFunc_i32_i32(py_func);
		auto nf = std::make_shared<NF_i32_i32>(name, std::move(cpp_func));
		return PyNautilusFunc_i32_i32 {std::move(nf)};
	});

	py::class_<PyNautilusFunc_i32_i32_i32>(m, "_NautilusFunc_i32_i32_i32")
	    .def("__call__", &PyNautilusFunc_i32_i32_i32::call);

	m.def("_create_nautilus_func_i32_i32_i32", [](const std::string& name, py::function py_func) {
		auto cpp_func = makeCppFunc_i32_i32_i32(py_func);
		auto nf = std::make_shared<NF_i32_i32_i32>(name, std::move(cpp_func));
		return PyNautilusFunc_i32_i32_i32 {std::move(nf)};
	});

	py::class_<PyNautilusFunc_i64_i64>(m, "_NautilusFunc_i64_i64").def("__call__", &PyNautilusFunc_i64_i64::call);

	m.def("_create_nautilus_func_i64_i64", [](const std::string& name, py::function py_func) {
		auto cpp_func = makeCppFunc_i64_i64(py_func);
		auto nf = std::make_shared<NF_i64_i64>(name, std::move(cpp_func));
		return PyNautilusFunc_i64_i64 {std::move(nf)};
	});

	py::class_<PyNautilusFunc_i64_i64_i64>(m, "_NautilusFunc_i64_i64_i64")
	    .def("__call__", &PyNautilusFunc_i64_i64_i64::call);

	m.def("_create_nautilus_func_i64_i64_i64", [](const std::string& name, py::function py_func) {
		auto cpp_func = makeCppFunc_i64_i64_i64(py_func);
		auto nf = std::make_shared<NF_i64_i64_i64>(name, std::move(cpp_func));
		return PyNautilusFunc_i64_i64_i64 {std::move(nf)};
	});

	py::class_<PyNautilusFunc_f64_f64>(m, "_NautilusFunc_f64_f64").def("__call__", &PyNautilusFunc_f64_f64::call);

	m.def("_create_nautilus_func_f64_f64", [](const std::string& name, py::function py_func) {
		auto cpp_func = makeCppFunc_f64_f64(py_func);
		auto nf = std::make_shared<NF_f64_f64>(name, std::move(cpp_func));
		return PyNautilusFunc_f64_f64 {std::move(nf)};
	});

	py::class_<PyNautilusFunc_f64_f64_f64>(m, "_NautilusFunc_f64_f64_f64")
	    .def("__call__", &PyNautilusFunc_f64_f64_f64::call);

	m.def("_create_nautilus_func_f64_f64_f64", [](const std::string& name, py::function py_func) {
		auto cpp_func = makeCppFunc_f64_f64_f64(py_func);
		auto nf = std::make_shared<NF_f64_f64_f64>(name, std::move(cpp_func));
		return PyNautilusFunc_f64_f64_f64 {std::move(nf)};
	});

	py::class_<PyNautilusFunc_f32_f32>(m, "_NautilusFunc_f32_f32").def("__call__", &PyNautilusFunc_f32_f32::call);

	m.def("_create_nautilus_func_f32_f32", [](const std::string& name, py::function py_func) {
		auto cpp_func = makeCppFunc_f32_f32(py_func);
		auto nf = std::make_shared<NF_f32_f32>(name, std::move(cpp_func));
		return PyNautilusFunc_f32_f32 {std::move(nf)};
	});

	py::class_<PyNautilusFunc_f32_f32_f32>(m, "_NautilusFunc_f32_f32_f32")
	    .def("__call__", &PyNautilusFunc_f32_f32_f32::call);

	m.def("_create_nautilus_func_f32_f32_f32", [](const std::string& name, py::function py_func) {
		auto cpp_func = makeCppFunc_f32_f32_f32(py_func);
		auto nf = std::make_shared<NF_f32_f32_f32>(name, std::move(cpp_func));
		return PyNautilusFunc_f32_f32_f32 {std::move(nf)};
	});

	py::class_<PyNautilusFunc_b_b>(m, "_NautilusFunc_b_b").def("__call__", &PyNautilusFunc_b_b::call);

	m.def("_create_nautilus_func_b_b", [](const std::string& name, py::function py_func) {
		auto cpp_func = makeCppFunc_b_b(py_func);
		auto nf = std::make_shared<NF_b_b>(name, std::move(cpp_func));
		return PyNautilusFunc_b_b {std::move(nf)};
	});

	py::class_<PyNautilusFunc_b_i32>(m, "_NautilusFunc_b_i32").def("__call__", &PyNautilusFunc_b_i32::call);

	m.def("_create_nautilus_func_b_i32", [](const std::string& name, py::function py_func) {
		auto cpp_func = makeCppFunc_b_i32(py_func);
		auto nf = std::make_shared<NF_b_i32>(name, std::move(cpp_func));
		return PyNautilusFunc_b_i32 {std::move(nf)};
	});
}
