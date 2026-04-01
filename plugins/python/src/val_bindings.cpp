#include "nautilus/function.hpp"
#include "nautilus/select.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_bool.hpp"
#include "py_object_helpers.hpp"
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace nautilus;

void bind_val_types(py::module_& m) {
	// --- ValBool (must be defined first, used as return type for comparisons) ---
	py::class_<val<bool>>(m, "ValBool")
	    .def(py::init<bool>(), py::arg("value") = false)
	    .def("__bool__", [](val<bool>& self) -> bool { return static_cast<bool>(self); })
	    // Logical operators (Python & | ~ map to logical AND OR NOT)
	    .def("__and__", [](const val<bool>& a, const val<bool>& b) { return a && b; })
	    .def("__rand__", [](const val<bool>& a, bool b) { return b && a; })
	    .def("__or__", [](const val<bool>& a, const val<bool>& b) { return a || b; })
	    .def("__ror__", [](const val<bool>& a, bool b) { return b || a; })
	    .def("__invert__", [](const val<bool>& a) { return !a; })
	    // Comparison
	    .def("__eq__", [](const val<bool>& a, const val<bool>& b) { return a == b; })
	    .def("__eq__", [](const val<bool>& a, bool b) { return a == b; })
	    .def("__ne__", [](const val<bool>& a, const val<bool>& b) { return a != b; })
	    .def("__ne__", [](const val<bool>& a, bool b) { return a != b; })
	    // Probability
	    .def("set_probability", &val<bool>::setIsTrueProbability, py::arg("prob"))
	    .def_property_readonly("probability", &val<bool>::getIsTrueProbability)
	    // Repr
	    .def("__repr__", [](const val<bool>&) { return "ValBool(...)"; });

	// --- ValInt32 ---
	py::class_<val<int32_t>>(m, "ValInt32")
	    .def(py::init<int32_t>(), py::arg("value") = 0)
	    // Arithmetic: val op val
	    .def("__add__", [](const val<int32_t>& a, const val<int32_t>& b) { return a + b; })
	    .def("__sub__", [](const val<int32_t>& a, const val<int32_t>& b) { return a - b; })
	    .def("__mul__", [](const val<int32_t>& a, const val<int32_t>& b) { return a * b; })
	    .def("__truediv__", [](const val<int32_t>& a, const val<int32_t>& b) { return a / b; })
	    .def("__floordiv__", [](const val<int32_t>& a, const val<int32_t>& b) { return a / b; })
	    .def("__mod__", [](const val<int32_t>& a, const val<int32_t>& b) { return a % b; })
	    // Arithmetic: val op raw
	    .def("__add__", [](const val<int32_t>& a, int32_t b) { return a + val<int32_t>(b); })
	    .def("__sub__", [](const val<int32_t>& a, int32_t b) { return a - val<int32_t>(b); })
	    .def("__mul__", [](const val<int32_t>& a, int32_t b) { return a * val<int32_t>(b); })
	    .def("__truediv__", [](const val<int32_t>& a, int32_t b) { return a / val<int32_t>(b); })
	    .def("__floordiv__", [](const val<int32_t>& a, int32_t b) { return a / val<int32_t>(b); })
	    .def("__mod__", [](const val<int32_t>& a, int32_t b) { return a % val<int32_t>(b); })
	    // Reverse arithmetic: raw op val
	    .def("__radd__", [](const val<int32_t>& a, int32_t b) { return val<int32_t>(b) + a; })
	    .def("__rsub__", [](const val<int32_t>& a, int32_t b) { return val<int32_t>(b) - a; })
	    .def("__rmul__", [](const val<int32_t>& a, int32_t b) { return val<int32_t>(b) * a; })
	    .def("__rtruediv__", [](const val<int32_t>& a, int32_t b) { return val<int32_t>(b) / a; })
	    .def("__rfloordiv__", [](const val<int32_t>& a, int32_t b) { return val<int32_t>(b) / a; })
	    .def("__rmod__", [](const val<int32_t>& a, int32_t b) { return val<int32_t>(b) % a; })
	    // Compound assignment — return by reference so the same C++ val object is reused.
	    // Critical for while-loop tracing: alive-variable hash must stay stable across
	    // loop iterations so the tracer can detect the loop back-edge.
	    .def(
	        "__iadd__",
	        [](val<int32_t>& a, const val<int32_t>& b) -> val<int32_t>& {
		        a = a + b;
		        return a;
	        },
	        py::return_value_policy::reference_internal)
	    .def(
	        "__iadd__",
	        [](val<int32_t>& a, int32_t b) -> val<int32_t>& {
		        a = a + val<int32_t>(b);
		        return a;
	        },
	        py::return_value_policy::reference_internal)
	    .def(
	        "__isub__",
	        [](val<int32_t>& a, const val<int32_t>& b) -> val<int32_t>& {
		        a = a - b;
		        return a;
	        },
	        py::return_value_policy::reference_internal)
	    .def(
	        "__isub__",
	        [](val<int32_t>& a, int32_t b) -> val<int32_t>& {
		        a = a - val<int32_t>(b);
		        return a;
	        },
	        py::return_value_policy::reference_internal)
	    .def(
	        "__imul__",
	        [](val<int32_t>& a, const val<int32_t>& b) -> val<int32_t>& {
		        a = a * b;
		        return a;
	        },
	        py::return_value_policy::reference_internal)
	    .def(
	        "__imul__",
	        [](val<int32_t>& a, int32_t b) -> val<int32_t>& {
		        a = a * val<int32_t>(b);
		        return a;
	        },
	        py::return_value_policy::reference_internal)
	    .def(
	        "__itruediv__",
	        [](val<int32_t>& a, const val<int32_t>& b) -> val<int32_t>& {
		        a = a / b;
		        return a;
	        },
	        py::return_value_policy::reference_internal)
	    .def(
	        "__imod__",
	        [](val<int32_t>& a, const val<int32_t>& b) -> val<int32_t>& {
		        a = a % b;
		        return a;
	        },
	        py::return_value_policy::reference_internal)
	    // Bitwise
	    .def("__lshift__", [](const val<int32_t>& a, const val<int32_t>& b) { return a << b; })
	    .def("__lshift__", [](const val<int32_t>& a, int32_t b) { return a << val<int32_t>(b); })
	    .def("__rshift__", [](const val<int32_t>& a, const val<int32_t>& b) { return a >> b; })
	    .def("__rshift__", [](const val<int32_t>& a, int32_t b) { return a >> val<int32_t>(b); })
	    .def("__and__", [](const val<int32_t>& a, const val<int32_t>& b) { return a & b; })
	    .def("__and__", [](const val<int32_t>& a, int32_t b) { return a & val<int32_t>(b); })
	    .def("__or__", [](const val<int32_t>& a, const val<int32_t>& b) { return a | b; })
	    .def("__or__", [](const val<int32_t>& a, int32_t b) { return a | val<int32_t>(b); })
	    .def("__xor__", [](const val<int32_t>& a, const val<int32_t>& b) { return a ^ b; })
	    .def("__xor__", [](const val<int32_t>& a, int32_t b) { return a ^ val<int32_t>(b); })
	    .def("__invert__", [](const val<int32_t>& a) { return ~a; })
	    // Unary
	    .def("__neg__", [](const val<int32_t>& a) { return -a; })
	    .def("__pos__", [](const val<int32_t>& a) { return +a; })
	    // Comparison → ValBool
	    .def("__eq__", [](const val<int32_t>& a, const val<int32_t>& b) { return a == b; })
	    .def("__eq__", [](const val<int32_t>& a, int32_t b) { return a == val<int32_t>(b); })
	    .def("__ne__", [](const val<int32_t>& a, const val<int32_t>& b) { return a != b; })
	    .def("__ne__", [](const val<int32_t>& a, int32_t b) { return a != val<int32_t>(b); })
	    .def("__lt__", [](const val<int32_t>& a, const val<int32_t>& b) { return a < b; })
	    .def("__lt__", [](const val<int32_t>& a, int32_t b) { return a < val<int32_t>(b); })
	    .def("__le__", [](const val<int32_t>& a, const val<int32_t>& b) { return a <= b; })
	    .def("__le__", [](const val<int32_t>& a, int32_t b) { return a <= val<int32_t>(b); })
	    .def("__gt__", [](const val<int32_t>& a, const val<int32_t>& b) { return a > b; })
	    .def("__gt__", [](const val<int32_t>& a, int32_t b) { return a > val<int32_t>(b); })
	    .def("__ge__", [](const val<int32_t>& a, const val<int32_t>& b) { return a >= b; })
	    .def("__ge__", [](const val<int32_t>& a, int32_t b) { return a >= val<int32_t>(b); })
	    // Casting
	    .def("cast_to_float64", [](const val<int32_t>& a) { return static_cast<val<double>>(a); })
	    .def("cast_to_int64", [](const val<int32_t>& a) { return static_cast<val<int64_t>>(a); })
	    // Assignment helper for traced reassignment
	    .def("assign", [](val<int32_t>& self, const val<int32_t>& other) { self = other; })
	    // Repr
	    .def("__repr__", [](const val<int32_t>&) { return "ValInt32(...)"; });

	// --- ValInt64 ---
	py::class_<val<int64_t>>(m, "ValInt64")
	    .def(py::init<int64_t>(), py::arg("value") = 0)
	    .def("__add__", [](const val<int64_t>& a, const val<int64_t>& b) { return a + b; })
	    .def("__add__", [](const val<int64_t>& a, int64_t b) { return a + val<int64_t>(b); })
	    .def("__radd__", [](const val<int64_t>& a, int64_t b) { return val<int64_t>(b) + a; })
	    .def("__sub__", [](const val<int64_t>& a, const val<int64_t>& b) { return a - b; })
	    .def("__sub__", [](const val<int64_t>& a, int64_t b) { return a - val<int64_t>(b); })
	    .def("__rsub__", [](const val<int64_t>& a, int64_t b) { return val<int64_t>(b) - a; })
	    .def("__mul__", [](const val<int64_t>& a, const val<int64_t>& b) { return a * b; })
	    .def("__mul__", [](const val<int64_t>& a, int64_t b) { return a * val<int64_t>(b); })
	    .def("__rmul__", [](const val<int64_t>& a, int64_t b) { return val<int64_t>(b) * a; })
	    .def("__truediv__", [](const val<int64_t>& a, const val<int64_t>& b) { return a / b; })
	    .def("__floordiv__", [](const val<int64_t>& a, const val<int64_t>& b) { return a / b; })
	    .def("__mod__", [](const val<int64_t>& a, const val<int64_t>& b) { return a % b; })
	    .def("__lshift__", [](const val<int64_t>& a, const val<int64_t>& b) { return a << b; })
	    .def("__rshift__", [](const val<int64_t>& a, const val<int64_t>& b) { return a >> b; })
	    .def("__and__", [](const val<int64_t>& a, const val<int64_t>& b) { return a & b; })
	    .def("__or__", [](const val<int64_t>& a, const val<int64_t>& b) { return a | b; })
	    .def("__xor__", [](const val<int64_t>& a, const val<int64_t>& b) { return a ^ b; })
	    .def("__invert__", [](const val<int64_t>& a) { return ~a; })
	    .def("__neg__", [](const val<int64_t>& a) { return -a; })
	    .def(
	        "__iadd__",
	        [](val<int64_t>& a, const val<int64_t>& b) -> val<int64_t>& {
		        a = a + b;
		        return a;
	        },
	        py::return_value_policy::reference_internal)
	    .def(
	        "__iadd__",
	        [](val<int64_t>& a, int64_t b) -> val<int64_t>& {
		        a = a + val<int64_t>(b);
		        return a;
	        },
	        py::return_value_policy::reference_internal)
	    .def(
	        "__isub__",
	        [](val<int64_t>& a, const val<int64_t>& b) -> val<int64_t>& {
		        a = a - b;
		        return a;
	        },
	        py::return_value_policy::reference_internal)
	    .def(
	        "__imul__",
	        [](val<int64_t>& a, const val<int64_t>& b) -> val<int64_t>& {
		        a = a * b;
		        return a;
	        },
	        py::return_value_policy::reference_internal)
	    .def("__eq__", [](const val<int64_t>& a, const val<int64_t>& b) { return a == b; })
	    .def("__eq__", [](const val<int64_t>& a, int64_t b) { return a == val<int64_t>(b); })
	    .def("__ne__", [](const val<int64_t>& a, const val<int64_t>& b) { return a != b; })
	    .def("__lt__", [](const val<int64_t>& a, const val<int64_t>& b) { return a < b; })
	    .def("__le__", [](const val<int64_t>& a, const val<int64_t>& b) { return a <= b; })
	    .def("__gt__", [](const val<int64_t>& a, const val<int64_t>& b) { return a > b; })
	    .def("__ge__", [](const val<int64_t>& a, const val<int64_t>& b) { return a >= b; })
	    .def("cast_to_int32", [](const val<int64_t>& a) { return static_cast<val<int32_t>>(a); })
	    .def("cast_to_float64", [](const val<int64_t>& a) { return static_cast<val<double>>(a); })
	    .def("assign", [](val<int64_t>& self, const val<int64_t>& other) { self = other; })
	    .def("__repr__", [](const val<int64_t>&) { return "ValInt64(...)"; });

	// --- ValFloat32 ---
	py::class_<val<float>>(m, "ValFloat32")
	    .def(py::init<float>(), py::arg("value") = 0.0f)
	    .def("__add__", [](const val<float>& a, const val<float>& b) { return a + b; })
	    .def("__add__", [](const val<float>& a, float b) { return a + val<float>(b); })
	    .def("__radd__", [](const val<float>& a, float b) { return val<float>(b) + a; })
	    .def("__sub__", [](const val<float>& a, const val<float>& b) { return a - b; })
	    .def("__mul__", [](const val<float>& a, const val<float>& b) { return a * b; })
	    .def("__truediv__", [](const val<float>& a, const val<float>& b) { return a / b; })
	    .def("__neg__", [](const val<float>& a) { return -a; })
	    .def(
	        "__iadd__",
	        [](val<float>& a, const val<float>& b) -> val<float>& {
		        a = a + b;
		        return a;
	        },
	        py::return_value_policy::reference_internal)
	    .def(
	        "__isub__",
	        [](val<float>& a, const val<float>& b) -> val<float>& {
		        a = a - b;
		        return a;
	        },
	        py::return_value_policy::reference_internal)
	    .def(
	        "__imul__",
	        [](val<float>& a, const val<float>& b) -> val<float>& {
		        a = a * b;
		        return a;
	        },
	        py::return_value_policy::reference_internal)
	    .def("__eq__", [](const val<float>& a, const val<float>& b) { return a == b; })
	    .def("__ne__", [](const val<float>& a, const val<float>& b) { return a != b; })
	    .def("__lt__", [](const val<float>& a, const val<float>& b) { return a < b; })
	    .def("__le__", [](const val<float>& a, const val<float>& b) { return a <= b; })
	    .def("__gt__", [](const val<float>& a, const val<float>& b) { return a > b; })
	    .def("__ge__", [](const val<float>& a, const val<float>& b) { return a >= b; })
	    .def("cast_to_float64", [](const val<float>& a) { return static_cast<val<double>>(a); })
	    .def("cast_to_int32", [](const val<float>& a) { return static_cast<val<int32_t>>(a); })
	    .def("assign", [](val<float>& self, const val<float>& other) { self = other; })
	    .def("__repr__", [](const val<float>&) { return "ValFloat32(...)"; });

	// --- ValFloat64 ---
	py::class_<val<double>>(m, "ValFloat64")
	    .def(py::init<double>(), py::arg("value") = 0.0)
	    // Arithmetic: val op val
	    .def("__add__", [](const val<double>& a, const val<double>& b) { return a + b; })
	    .def("__sub__", [](const val<double>& a, const val<double>& b) { return a - b; })
	    .def("__mul__", [](const val<double>& a, const val<double>& b) { return a * b; })
	    .def("__truediv__", [](const val<double>& a, const val<double>& b) { return a / b; })
	    // Arithmetic: val op raw
	    .def("__add__", [](const val<double>& a, double b) { return a + val<double>(b); })
	    .def("__sub__", [](const val<double>& a, double b) { return a - val<double>(b); })
	    .def("__mul__", [](const val<double>& a, double b) { return a * val<double>(b); })
	    .def("__truediv__", [](const val<double>& a, double b) { return a / val<double>(b); })
	    // Reverse arithmetic: raw op val
	    .def("__radd__", [](const val<double>& a, double b) { return val<double>(b) + a; })
	    .def("__rsub__", [](const val<double>& a, double b) { return val<double>(b) - a; })
	    .def("__rmul__", [](const val<double>& a, double b) { return val<double>(b) * a; })
	    .def("__rtruediv__", [](const val<double>& a, double b) { return val<double>(b) / a; })
	    // Compound assignment
	    .def(
	        "__iadd__",
	        [](val<double>& a, const val<double>& b) -> val<double>& {
		        a = a + b;
		        return a;
	        },
	        py::return_value_policy::reference_internal)
	    .def(
	        "__iadd__",
	        [](val<double>& a, double b) -> val<double>& {
		        a = a + val<double>(b);
		        return a;
	        },
	        py::return_value_policy::reference_internal)
	    .def(
	        "__isub__",
	        [](val<double>& a, const val<double>& b) -> val<double>& {
		        a = a - b;
		        return a;
	        },
	        py::return_value_policy::reference_internal)
	    .def(
	        "__imul__",
	        [](val<double>& a, const val<double>& b) -> val<double>& {
		        a = a * b;
		        return a;
	        },
	        py::return_value_policy::reference_internal)
	    .def(
	        "__itruediv__",
	        [](val<double>& a, const val<double>& b) -> val<double>& {
		        a = a / b;
		        return a;
	        },
	        py::return_value_policy::reference_internal)
	    // Unary
	    .def("__neg__", [](const val<double>& a) { return -a; })
	    .def("__pos__", [](const val<double>& a) { return +a; })
	    // Comparison → ValBool
	    .def("__eq__", [](const val<double>& a, const val<double>& b) { return a == b; })
	    .def("__eq__", [](const val<double>& a, double b) { return a == val<double>(b); })
	    .def("__ne__", [](const val<double>& a, const val<double>& b) { return a != b; })
	    .def("__ne__", [](const val<double>& a, double b) { return a != val<double>(b); })
	    .def("__lt__", [](const val<double>& a, const val<double>& b) { return a < b; })
	    .def("__lt__", [](const val<double>& a, double b) { return a < val<double>(b); })
	    .def("__le__", [](const val<double>& a, const val<double>& b) { return a <= b; })
	    .def("__le__", [](const val<double>& a, double b) { return a <= val<double>(b); })
	    .def("__gt__", [](const val<double>& a, const val<double>& b) { return a > b; })
	    .def("__gt__", [](const val<double>& a, double b) { return a > val<double>(b); })
	    .def("__ge__", [](const val<double>& a, const val<double>& b) { return a >= b; })
	    .def("__ge__", [](const val<double>& a, double b) { return a >= val<double>(b); })
	    // Casting
	    .def("cast_to_int32", [](const val<double>& a) { return static_cast<val<int32_t>>(a); })
	    .def("cast_to_int64", [](const val<double>& a) { return static_cast<val<int64_t>>(a); })
	    .def("cast_to_float32", [](const val<double>& a) { return static_cast<val<float>>(a); })
	    // Assignment helper for traced reassignment
	    .def("assign", [](val<double>& self, const val<double>& other) { self = other; })
	    // Repr
	    .def("__repr__", [](const val<double>&) { return "ValFloat64(...)"; });

	// --- select() ---
	m.def(
	    "select", [](val<bool> cond, val<int32_t> t, val<int32_t> f) { return nautilus::select(cond, t, f); },
	    "Select between two int32 values based on a boolean condition");
	m.def(
	    "select", [](val<bool> cond, val<int64_t> t, val<int64_t> f) { return nautilus::select(cond, t, f); },
	    "Select between two int64 values based on a boolean condition");
	m.def(
	    "select", [](val<bool> cond, val<float> t, val<float> f) { return nautilus::select(cond, t, f); },
	    "Select between two float32 values based on a boolean condition");
	m.def(
	    "select", [](val<bool> cond, val<double> t, val<double> f) { return nautilus::select(cond, t, f); },
	    "Select between two float64 values based on a boolean condition");

	// --- ValObject (val<void*>) — wraps arbitrary Python objects ----------------
	// All operations are forwarded to the Python runtime via invoke() trampolines.
	// During tracing, invoke() records a CALL operation; during compiled execution,
	// it calls the actual trampoline which performs the Python C-API operation.

	py::class_<val<void*>>(m, "ValObject")
	    .def(py::init([](py::object obj) { return val<void*>(static_cast<void*>(obj.ptr())); }), py::arg("value"))

	    // ── Arithmetic ──────────────────────────────────────────────────────
	    .def("__add__",
	         [](val<void*>& a, val<void*>& b) {
		         return invoke(
		             +[](void* x, void* y) -> void* {
			             auto gstate = PyGILState_Ensure();
			             auto* r = PyNumber_Add(static_cast<PyObject*>(x), static_cast<PyObject*>(y));
			             PyGILState_Release(gstate);
			             return arenaTrack(r);
		             },
		             a, b);
	         })
	    .def("__add__",
	         [](val<void*>& a, py::object b) {
		         val<void*> bv(static_cast<void*>(b.ptr()));
		         return invoke(
		             +[](void* x, void* y) -> void* {
			             auto gstate = PyGILState_Ensure();
			             auto* r = PyNumber_Add(static_cast<PyObject*>(x), static_cast<PyObject*>(y));
			             PyGILState_Release(gstate);
			             return arenaTrack(r);
		             },
		             a, bv);
	         })
	    .def("__radd__",
	         [](val<void*>& a, py::object b) {
		         val<void*> bv(static_cast<void*>(b.ptr()));
		         return invoke(
		             +[](void* x, void* y) -> void* {
			             auto gstate = PyGILState_Ensure();
			             auto* r = PyNumber_Add(static_cast<PyObject*>(x), static_cast<PyObject*>(y));
			             PyGILState_Release(gstate);
			             return arenaTrack(r);
		             },
		             bv, a);
	         })

	    .def("__sub__",
	         [](val<void*>& a, val<void*>& b) {
		         return invoke(
		             +[](void* x, void* y) -> void* {
			             auto gstate = PyGILState_Ensure();
			             auto* r = PyNumber_Subtract(static_cast<PyObject*>(x), static_cast<PyObject*>(y));
			             PyGILState_Release(gstate);
			             return arenaTrack(r);
		             },
		             a, b);
	         })
	    .def("__sub__",
	         [](val<void*>& a, py::object b) {
		         val<void*> bv(static_cast<void*>(b.ptr()));
		         return invoke(
		             +[](void* x, void* y) -> void* {
			             auto gstate = PyGILState_Ensure();
			             auto* r = PyNumber_Subtract(static_cast<PyObject*>(x), static_cast<PyObject*>(y));
			             PyGILState_Release(gstate);
			             return arenaTrack(r);
		             },
		             a, bv);
	         })
	    .def("__rsub__",
	         [](val<void*>& a, py::object b) {
		         val<void*> bv(static_cast<void*>(b.ptr()));
		         return invoke(
		             +[](void* x, void* y) -> void* {
			             auto gstate = PyGILState_Ensure();
			             auto* r = PyNumber_Subtract(static_cast<PyObject*>(x), static_cast<PyObject*>(y));
			             PyGILState_Release(gstate);
			             return arenaTrack(r);
		             },
		             bv, a);
	         })

	    .def("__mul__",
	         [](val<void*>& a, val<void*>& b) {
		         return invoke(
		             +[](void* x, void* y) -> void* {
			             auto gstate = PyGILState_Ensure();
			             auto* r = PyNumber_Multiply(static_cast<PyObject*>(x), static_cast<PyObject*>(y));
			             PyGILState_Release(gstate);
			             return arenaTrack(r);
		             },
		             a, b);
	         })
	    .def("__mul__",
	         [](val<void*>& a, py::object b) {
		         val<void*> bv(static_cast<void*>(b.ptr()));
		         return invoke(
		             +[](void* x, void* y) -> void* {
			             auto gstate = PyGILState_Ensure();
			             auto* r = PyNumber_Multiply(static_cast<PyObject*>(x), static_cast<PyObject*>(y));
			             PyGILState_Release(gstate);
			             return arenaTrack(r);
		             },
		             a, bv);
	         })
	    .def("__rmul__",
	         [](val<void*>& a, py::object b) {
		         val<void*> bv(static_cast<void*>(b.ptr()));
		         return invoke(
		             +[](void* x, void* y) -> void* {
			             auto gstate = PyGILState_Ensure();
			             auto* r = PyNumber_Multiply(static_cast<PyObject*>(x), static_cast<PyObject*>(y));
			             PyGILState_Release(gstate);
			             return arenaTrack(r);
		             },
		             bv, a);
	         })

	    .def("__truediv__",
	         [](val<void*>& a, val<void*>& b) {
		         return invoke(
		             +[](void* x, void* y) -> void* {
			             auto gstate = PyGILState_Ensure();
			             auto* r = PyNumber_TrueDivide(static_cast<PyObject*>(x), static_cast<PyObject*>(y));
			             PyGILState_Release(gstate);
			             return arenaTrack(r);
		             },
		             a, b);
	         })
	    .def("__truediv__",
	         [](val<void*>& a, py::object b) {
		         val<void*> bv(static_cast<void*>(b.ptr()));
		         return invoke(
		             +[](void* x, void* y) -> void* {
			             auto gstate = PyGILState_Ensure();
			             auto* r = PyNumber_TrueDivide(static_cast<PyObject*>(x), static_cast<PyObject*>(y));
			             PyGILState_Release(gstate);
			             return arenaTrack(r);
		             },
		             a, bv);
	         })

	    .def("__floordiv__",
	         [](val<void*>& a, val<void*>& b) {
		         return invoke(
		             +[](void* x, void* y) -> void* {
			             auto gstate = PyGILState_Ensure();
			             auto* r = PyNumber_FloorDivide(static_cast<PyObject*>(x), static_cast<PyObject*>(y));
			             PyGILState_Release(gstate);
			             return arenaTrack(r);
		             },
		             a, b);
	         })

	    .def("__mod__",
	         [](val<void*>& a, val<void*>& b) {
		         return invoke(
		             +[](void* x, void* y) -> void* {
			             auto gstate = PyGILState_Ensure();
			             auto* r = PyNumber_Remainder(static_cast<PyObject*>(x), static_cast<PyObject*>(y));
			             PyGILState_Release(gstate);
			             return arenaTrack(r);
		             },
		             a, b);
	         })

	    .def("__neg__",
	         [](val<void*>& a) {
		         return invoke(
		             +[](void* x) -> void* {
			             auto gstate = PyGILState_Ensure();
			             auto* r = PyNumber_Negative(static_cast<PyObject*>(x));
			             PyGILState_Release(gstate);
			             return arenaTrack(r);
		             },
		             a);
	         })

	    // ── Compound assignment ─────────────────────────────────────────────
	    .def(
	        "__iadd__",
	        [](val<void*>& a, val<void*>& b) -> val<void*>& {
		        a = invoke(
		            +[](void* x, void* y) -> void* {
			            auto gstate = PyGILState_Ensure();
			            auto* r = PyNumber_Add(static_cast<PyObject*>(x), static_cast<PyObject*>(y));
			            PyGILState_Release(gstate);
			            return arenaTrack(r);
		            },
		            a, b);
		        return a;
	        },
	        py::return_value_policy::reference_internal)
	    .def(
	        "__iadd__",
	        [](val<void*>& a, py::object b) -> val<void*>& {
		        val<void*> bv(static_cast<void*>(b.ptr()));
		        a = invoke(
		            +[](void* x, void* y) -> void* {
			            auto gstate = PyGILState_Ensure();
			            auto* r = PyNumber_Add(static_cast<PyObject*>(x), static_cast<PyObject*>(y));
			            PyGILState_Release(gstate);
			            return arenaTrack(r);
		            },
		            a, bv);
		        return a;
	        },
	        py::return_value_policy::reference_internal)
	    .def(
	        "__isub__",
	        [](val<void*>& a, val<void*>& b) -> val<void*>& {
		        a = invoke(
		            +[](void* x, void* y) -> void* {
			            auto gstate = PyGILState_Ensure();
			            auto* r = PyNumber_Subtract(static_cast<PyObject*>(x), static_cast<PyObject*>(y));
			            PyGILState_Release(gstate);
			            return arenaTrack(r);
		            },
		            a, b);
		        return a;
	        },
	        py::return_value_policy::reference_internal)
	    .def(
	        "__imul__",
	        [](val<void*>& a, val<void*>& b) -> val<void*>& {
		        a = invoke(
		            +[](void* x, void* y) -> void* {
			            auto gstate = PyGILState_Ensure();
			            auto* r = PyNumber_Multiply(static_cast<PyObject*>(x), static_cast<PyObject*>(y));
			            PyGILState_Release(gstate);
			            return arenaTrack(r);
		            },
		            a, b);
		        return a;
	        },
	        py::return_value_policy::reference_internal)

	    // ── Comparison → ValBool ────────────────────────────────────────────
	    .def("__eq__",
	         [](val<void*>& a, val<void*>& b) {
		         return invoke(
		             +[](void* x, void* y) -> bool {
			             auto gstate = PyGILState_Ensure();
			             int r = PyObject_RichCompareBool(static_cast<PyObject*>(x), static_cast<PyObject*>(y), Py_EQ);
			             PyGILState_Release(gstate);
			             return r == 1;
		             },
		             a, b);
	         })
	    .def("__eq__",
	         [](val<void*>& a, py::object b) {
		         val<void*> bv(static_cast<void*>(b.ptr()));
		         return invoke(
		             +[](void* x, void* y) -> bool {
			             auto gstate = PyGILState_Ensure();
			             int r = PyObject_RichCompareBool(static_cast<PyObject*>(x), static_cast<PyObject*>(y), Py_EQ);
			             PyGILState_Release(gstate);
			             return r == 1;
		             },
		             a, bv);
	         })
	    .def("__ne__",
	         [](val<void*>& a, val<void*>& b) {
		         return invoke(
		             +[](void* x, void* y) -> bool {
			             auto gstate = PyGILState_Ensure();
			             int r = PyObject_RichCompareBool(static_cast<PyObject*>(x), static_cast<PyObject*>(y), Py_NE);
			             PyGILState_Release(gstate);
			             return r == 1;
		             },
		             a, b);
	         })
	    .def("__lt__",
	         [](val<void*>& a, val<void*>& b) {
		         return invoke(
		             +[](void* x, void* y) -> bool {
			             auto gstate = PyGILState_Ensure();
			             int r = PyObject_RichCompareBool(static_cast<PyObject*>(x), static_cast<PyObject*>(y), Py_LT);
			             PyGILState_Release(gstate);
			             return r == 1;
		             },
		             a, b);
	         })
	    .def("__lt__",
	         [](val<void*>& a, py::object b) {
		         val<void*> bv(static_cast<void*>(b.ptr()));
		         return invoke(
		             +[](void* x, void* y) -> bool {
			             auto gstate = PyGILState_Ensure();
			             int r = PyObject_RichCompareBool(static_cast<PyObject*>(x), static_cast<PyObject*>(y), Py_LT);
			             PyGILState_Release(gstate);
			             return r == 1;
		             },
		             a, bv);
	         })
	    .def("__le__",
	         [](val<void*>& a, val<void*>& b) {
		         return invoke(
		             +[](void* x, void* y) -> bool {
			             auto gstate = PyGILState_Ensure();
			             int r = PyObject_RichCompareBool(static_cast<PyObject*>(x), static_cast<PyObject*>(y), Py_LE);
			             PyGILState_Release(gstate);
			             return r == 1;
		             },
		             a, b);
	         })
	    .def("__gt__",
	         [](val<void*>& a, val<void*>& b) {
		         return invoke(
		             +[](void* x, void* y) -> bool {
			             auto gstate = PyGILState_Ensure();
			             int r = PyObject_RichCompareBool(static_cast<PyObject*>(x), static_cast<PyObject*>(y), Py_GT);
			             PyGILState_Release(gstate);
			             return r == 1;
		             },
		             a, b);
	         })
	    .def("__gt__",
	         [](val<void*>& a, py::object b) {
		         val<void*> bv(static_cast<void*>(b.ptr()));
		         return invoke(
		             +[](void* x, void* y) -> bool {
			             auto gstate = PyGILState_Ensure();
			             int r = PyObject_RichCompareBool(static_cast<PyObject*>(x), static_cast<PyObject*>(y), Py_GT);
			             PyGILState_Release(gstate);
			             return r == 1;
		             },
		             a, bv);
	         })
	    .def("__ge__",
	         [](val<void*>& a, val<void*>& b) {
		         return invoke(
		             +[](void* x, void* y) -> bool {
			             auto gstate = PyGILState_Ensure();
			             int r = PyObject_RichCompareBool(static_cast<PyObject*>(x), static_cast<PyObject*>(y), Py_GE);
			             PyGILState_Release(gstate);
			             return r == 1;
		             },
		             a, b);
	         })

	    // ── Bool conversion (triggers traceBool for if/while) ───────────────
	    .def("__bool__",
	         [](val<void*>& a) -> bool {
		         auto result = invoke(
		             +[](void* x) -> bool {
			             auto gstate = PyGILState_Ensure();
			             int r = PyObject_IsTrue(static_cast<PyObject*>(x));
			             PyGILState_Release(gstate);
			             return r == 1;
		             },
		             a);
		         return static_cast<bool>(result);
	         })

	    // ── Method calls: x.call("method", args...) ──────────────────────────
	    .def("call",
	         [](val<void*>& self, py::str name, py::args args) -> val<void*> {
		         val<void*> name_val(static_cast<void*>(name.ptr()));

		         auto toVal = [](py::handle arg) -> val<void*> {
			         if (py::isinstance<val<void*>>(arg)) {
				         return arg.cast<val<void*>>();
			         }
			         return val<void*>(static_cast<void*>(arg.ptr()));
		         };

		         switch (args.size()) {
		         case 0:
			         return invoke(
			             +[](void* o, void* n) -> void* {
				             auto gstate = PyGILState_Ensure();
				             auto* method = PyObject_GetAttr(static_cast<PyObject*>(o), static_cast<PyObject*>(n));
				             PyObject* r = nullptr;
				             if (method) {
					             r = PyObject_CallObject(method, nullptr);
					             Py_DECREF(method);
				             }
				             PyGILState_Release(gstate);
				             return arenaTrack(r);
			             },
			             self, name_val);
		         case 1: {
			         val<void*> a0 = toVal(args[0]);
			         return invoke(
			             +[](void* o, void* n, void* a) -> void* {
				             auto gstate = PyGILState_Ensure();
				             auto* method = PyObject_GetAttr(static_cast<PyObject*>(o), static_cast<PyObject*>(n));
				             PyObject* r = nullptr;
				             if (method) {
					             auto* t = PyTuple_Pack(1, static_cast<PyObject*>(a));
					             r = PyObject_Call(method, t, nullptr);
					             Py_DECREF(t);
					             Py_DECREF(method);
				             }
				             PyGILState_Release(gstate);
				             return arenaTrack(r);
			             },
			             self, name_val, a0);
		         }
		         case 2: {
			         val<void*> a0 = toVal(args[0]);
			         val<void*> a1 = toVal(args[1]);
			         return invoke(
			             +[](void* o, void* n, void* a, void* b) -> void* {
				             auto gstate = PyGILState_Ensure();
				             auto* method = PyObject_GetAttr(static_cast<PyObject*>(o), static_cast<PyObject*>(n));
				             PyObject* r = nullptr;
				             if (method) {
					             auto* t = PyTuple_Pack(2, static_cast<PyObject*>(a), static_cast<PyObject*>(b));
					             r = PyObject_Call(method, t, nullptr);
					             Py_DECREF(t);
					             Py_DECREF(method);
				             }
				             PyGILState_Release(gstate);
				             return arenaTrack(r);
			             },
			             self, name_val, a0, a1);
		         }
		         case 3: {
			         val<void*> a0 = toVal(args[0]);
			         val<void*> a1 = toVal(args[1]);
			         val<void*> a2 = toVal(args[2]);
			         return invoke(
			             +[](void* o, void* n, void* a, void* b, void* c) -> void* {
				             auto gstate = PyGILState_Ensure();
				             auto* method = PyObject_GetAttr(static_cast<PyObject*>(o), static_cast<PyObject*>(n));
				             PyObject* r = nullptr;
				             if (method) {
					             auto* t = PyTuple_Pack(3, static_cast<PyObject*>(a), static_cast<PyObject*>(b),
					                                    static_cast<PyObject*>(c));
					             r = PyObject_Call(method, t, nullptr);
					             Py_DECREF(t);
					             Py_DECREF(method);
				             }
				             PyGILState_Release(gstate);
				             return arenaTrack(r);
			             },
			             self, name_val, a0, a1, a2);
		         }
		         default:
			         throw std::runtime_error("call() supports at most 3 arguments");
		         }
	         })

	    // Explicit attribute access: x.getattr("name") → val<void*>
	    .def("getattr",
	         [](val<void*>& self, py::str name) {
		         val<void*> name_val(static_cast<void*>(name.ptr()));
		         return invoke(
		             +[](void* o, void* n) -> void* {
			             auto gstate = PyGILState_Ensure();
			             auto* r = PyObject_GetAttr(static_cast<PyObject*>(o), static_cast<PyObject*>(n));
			             PyGILState_Release(gstate);
			             return arenaTrack(r);
		             },
		             self, name_val);
	         })

	    // ── Assignment ──────────────────────────────────────────────────────
	    .def("assign", [](val<void*>& self, const val<void*>& other) { self = other; })

	    .def("__repr__", [](const val<void*>&) { return "ValObject(...)"; });
}
