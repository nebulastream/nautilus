#include <pybind11/pybind11.h>

namespace py = pybind11;

void bind_val_types(py::module_& m);
void bind_engine(py::module_& m);

PYBIND11_MODULE(nautilus_native, m) {
	m.doc() = "Python bindings for the Nautilus tracing JIT compiler";
	bind_val_types(m);
	bind_engine(m);
}
