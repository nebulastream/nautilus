#include "nautilus/select.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_bool.hpp"
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
	    // Compound assignment
	    .def("__iadd__",
	         [](val<int32_t>& a, const val<int32_t>& b) {
		         a = a + b;
		         return a;
	         })
	    .def("__iadd__",
	         [](val<int32_t>& a, int32_t b) {
		         a = a + val<int32_t>(b);
		         return a;
	         })
	    .def("__isub__",
	         [](val<int32_t>& a, const val<int32_t>& b) {
		         a = a - b;
		         return a;
	         })
	    .def("__isub__",
	         [](val<int32_t>& a, int32_t b) {
		         a = a - val<int32_t>(b);
		         return a;
	         })
	    .def("__imul__",
	         [](val<int32_t>& a, const val<int32_t>& b) {
		         a = a * b;
		         return a;
	         })
	    .def("__imul__",
	         [](val<int32_t>& a, int32_t b) {
		         a = a * val<int32_t>(b);
		         return a;
	         })
	    .def("__itruediv__",
	         [](val<int32_t>& a, const val<int32_t>& b) {
		         a = a / b;
		         return a;
	         })
	    .def("__imod__",
	         [](val<int32_t>& a, const val<int32_t>& b) {
		         a = a % b;
		         return a;
	         })
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
	    .def("__iadd__",
	         [](val<int64_t>& a, const val<int64_t>& b) {
		         a = a + b;
		         return a;
	         })
	    .def("__iadd__",
	         [](val<int64_t>& a, int64_t b) {
		         a = a + val<int64_t>(b);
		         return a;
	         })
	    .def("__isub__",
	         [](val<int64_t>& a, const val<int64_t>& b) {
		         a = a - b;
		         return a;
	         })
	    .def("__imul__",
	         [](val<int64_t>& a, const val<int64_t>& b) {
		         a = a * b;
		         return a;
	         })
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
	    .def("__iadd__",
	         [](val<float>& a, const val<float>& b) {
		         a = a + b;
		         return a;
	         })
	    .def("__isub__",
	         [](val<float>& a, const val<float>& b) {
		         a = a - b;
		         return a;
	         })
	    .def("__imul__",
	         [](val<float>& a, const val<float>& b) {
		         a = a * b;
		         return a;
	         })
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
	    .def("__iadd__",
	         [](val<double>& a, const val<double>& b) {
		         a = a + b;
		         return a;
	         })
	    .def("__iadd__",
	         [](val<double>& a, double b) {
		         a = a + val<double>(b);
		         return a;
	         })
	    .def("__isub__",
	         [](val<double>& a, const val<double>& b) {
		         a = a - b;
		         return a;
	         })
	    .def("__imul__",
	         [](val<double>& a, const val<double>& b) {
		         a = a * b;
		         return a;
	         })
	    .def("__itruediv__",
	         [](val<double>& a, const val<double>& b) {
		         a = a / b;
		         return a;
	         })
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
}
