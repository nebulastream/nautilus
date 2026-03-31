#include "nautilus/tracing/symbolic_execution/TraceTerminationException.hpp"
#include <pybind11/pybind11.h>

namespace py = pybind11;

void bind_val_types(py::module_& m);
void bind_engine(py::module_& m);

// Python exception type for TraceTerminationException.
// This allows the C++ exception to propagate through Python frames
// and be re-thrown as the original C++ exception on the other side.
static PyObject* TraceTerminationExceptionType = nullptr;

PyObject* getTraceTerminationExceptionType() {
	return TraceTerminationExceptionType;
}

PYBIND11_MODULE(nautilus_native, m) {
	m.doc() = "Python bindings for the Nautilus tracing JIT compiler";

	// Register TraceTerminationException so it propagates through Python
	// instead of being converted to a generic RuntimeError.
	static auto exc =
	    py::register_exception<nautilus::tracing::TraceTerminationException>(m, "_TraceTerminationException");
	TraceTerminationExceptionType = exc.ptr();

	bind_val_types(m);
	bind_engine(m);
}
