#pragma once
#include <Python.h>
#include <vector>

// ── Reference-counting arena ─────────────────────────────────────────────────
// Thread-local arena for tracking new PyObject* references created by
// trampolines during compiled execution.  After the compiled function
// returns, the caller Py_XINCREFs the result and clears the arena.

inline thread_local std::vector<PyObject*> g_py_ref_arena;

/// Track a new PyObject* reference in the arena and return it as void*.
inline void* arenaTrack(PyObject* obj) {
	if (obj) {
		g_py_ref_arena.push_back(obj);
	}
	return static_cast<void*>(obj);
}

/// Py_XDECREF all tracked references and clear the arena.
inline void arenaClear() {
	for (auto* p : g_py_ref_arena) {
		Py_XDECREF(p);
	}
	g_py_ref_arena.clear();
}

// ── Binary arithmetic trampolines ────────────────────────────────────────────
// Each acquires the GIL, calls the Python C-API, releases the GIL, and
// tracks the result in the arena.  These are plain function pointers suitable
// for invoke().

inline void* pyAdd(void* x, void* y) {
	auto gstate = PyGILState_Ensure();
	auto* r = PyNumber_Add(static_cast<PyObject*>(x), static_cast<PyObject*>(y));
	PyGILState_Release(gstate);
	return arenaTrack(r);
}

inline void* pySub(void* x, void* y) {
	auto gstate = PyGILState_Ensure();
	auto* r = PyNumber_Subtract(static_cast<PyObject*>(x), static_cast<PyObject*>(y));
	PyGILState_Release(gstate);
	return arenaTrack(r);
}

inline void* pyMul(void* x, void* y) {
	auto gstate = PyGILState_Ensure();
	auto* r = PyNumber_Multiply(static_cast<PyObject*>(x), static_cast<PyObject*>(y));
	PyGILState_Release(gstate);
	return arenaTrack(r);
}

inline void* pyTrueDiv(void* x, void* y) {
	auto gstate = PyGILState_Ensure();
	auto* r = PyNumber_TrueDivide(static_cast<PyObject*>(x), static_cast<PyObject*>(y));
	PyGILState_Release(gstate);
	return arenaTrack(r);
}

inline void* pyFloorDiv(void* x, void* y) {
	auto gstate = PyGILState_Ensure();
	auto* r = PyNumber_FloorDivide(static_cast<PyObject*>(x), static_cast<PyObject*>(y));
	PyGILState_Release(gstate);
	return arenaTrack(r);
}

inline void* pyMod(void* x, void* y) {
	auto gstate = PyGILState_Ensure();
	auto* r = PyNumber_Remainder(static_cast<PyObject*>(x), static_cast<PyObject*>(y));
	PyGILState_Release(gstate);
	return arenaTrack(r);
}

// ── Unary arithmetic trampoline ──────────────────────────────────────────────

inline void* pyNeg(void* x) {
	auto gstate = PyGILState_Ensure();
	auto* r = PyNumber_Negative(static_cast<PyObject*>(x));
	PyGILState_Release(gstate);
	return arenaTrack(r);
}

// ── Comparison trampolines ───────────────────────────────────────────────────
// PyObject_RichCompareBool returns -1 on error; we clear the error and
// return false rather than crashing or silently propagating a stale exception.

inline bool pyEq(void* x, void* y) {
	auto gstate = PyGILState_Ensure();
	int r = PyObject_RichCompareBool(static_cast<PyObject*>(x), static_cast<PyObject*>(y), Py_EQ);
	if (r == -1) {
		PyErr_Clear();
	}
	PyGILState_Release(gstate);
	return r == 1;
}

inline bool pyNe(void* x, void* y) {
	auto gstate = PyGILState_Ensure();
	int r = PyObject_RichCompareBool(static_cast<PyObject*>(x), static_cast<PyObject*>(y), Py_NE);
	if (r == -1) {
		PyErr_Clear();
	}
	PyGILState_Release(gstate);
	return r == 1;
}

inline bool pyLt(void* x, void* y) {
	auto gstate = PyGILState_Ensure();
	int r = PyObject_RichCompareBool(static_cast<PyObject*>(x), static_cast<PyObject*>(y), Py_LT);
	if (r == -1) {
		PyErr_Clear();
	}
	PyGILState_Release(gstate);
	return r == 1;
}

inline bool pyLe(void* x, void* y) {
	auto gstate = PyGILState_Ensure();
	int r = PyObject_RichCompareBool(static_cast<PyObject*>(x), static_cast<PyObject*>(y), Py_LE);
	if (r == -1) {
		PyErr_Clear();
	}
	PyGILState_Release(gstate);
	return r == 1;
}

inline bool pyGt(void* x, void* y) {
	auto gstate = PyGILState_Ensure();
	int r = PyObject_RichCompareBool(static_cast<PyObject*>(x), static_cast<PyObject*>(y), Py_GT);
	if (r == -1) {
		PyErr_Clear();
	}
	PyGILState_Release(gstate);
	return r == 1;
}

inline bool pyGe(void* x, void* y) {
	auto gstate = PyGILState_Ensure();
	int r = PyObject_RichCompareBool(static_cast<PyObject*>(x), static_cast<PyObject*>(y), Py_GE);
	if (r == -1) {
		PyErr_Clear();
	}
	PyGILState_Release(gstate);
	return r == 1;
}

// ── Bool conversion trampoline ───────────────────────────────────────────────

inline bool pyIsTrue(void* x) {
	auto gstate = PyGILState_Ensure();
	int r = PyObject_IsTrue(static_cast<PyObject*>(x));
	if (r == -1) {
		PyErr_Clear();
	}
	PyGILState_Release(gstate);
	return r == 1;
}

// ── Attribute access trampoline ──────────────────────────────────────────────

inline void* pyGetAttr(void* o, void* n) {
	auto gstate = PyGILState_Ensure();
	auto* r = PyObject_GetAttr(static_cast<PyObject*>(o), static_cast<PyObject*>(n));
	PyGILState_Release(gstate);
	return arenaTrack(r);
}

// ── Method call trampolines ──────────────────────────────────────────────────
// Each variant handles a fixed number of arguments (0–3) because invoke()
// requires a concrete function-pointer signature.

inline void* pyCallMethod0(void* o, void* n) {
	auto gstate = PyGILState_Ensure();
	auto* method = PyObject_GetAttr(static_cast<PyObject*>(o), static_cast<PyObject*>(n));
	PyObject* r = nullptr;
	if (method) {
		r = PyObject_CallObject(method, nullptr);
		Py_DECREF(method);
	}
	PyGILState_Release(gstate);
	return arenaTrack(r);
}

inline void* pyCallMethod1(void* o, void* n, void* a) {
	auto gstate = PyGILState_Ensure();
	auto* method = PyObject_GetAttr(static_cast<PyObject*>(o), static_cast<PyObject*>(n));
	PyObject* r = nullptr;
	if (method) {
		auto* t = PyTuple_Pack(1, static_cast<PyObject*>(a));
		if (t) {
			r = PyObject_Call(method, t, nullptr);
			Py_DECREF(t);
		}
		Py_DECREF(method);
	}
	PyGILState_Release(gstate);
	return arenaTrack(r);
}

inline void* pyCallMethod2(void* o, void* n, void* a, void* b) {
	auto gstate = PyGILState_Ensure();
	auto* method = PyObject_GetAttr(static_cast<PyObject*>(o), static_cast<PyObject*>(n));
	PyObject* r = nullptr;
	if (method) {
		auto* t = PyTuple_Pack(2, static_cast<PyObject*>(a), static_cast<PyObject*>(b));
		if (t) {
			r = PyObject_Call(method, t, nullptr);
			Py_DECREF(t);
		}
		Py_DECREF(method);
	}
	PyGILState_Release(gstate);
	return arenaTrack(r);
}

inline void* pyCallMethod3(void* o, void* n, void* a, void* b, void* c) {
	auto gstate = PyGILState_Ensure();
	auto* method = PyObject_GetAttr(static_cast<PyObject*>(o), static_cast<PyObject*>(n));
	PyObject* r = nullptr;
	if (method) {
		auto* t = PyTuple_Pack(3, static_cast<PyObject*>(a), static_cast<PyObject*>(b), static_cast<PyObject*>(c));
		if (t) {
			r = PyObject_Call(method, t, nullptr);
			Py_DECREF(t);
		}
		Py_DECREF(method);
	}
	PyGILState_Release(gstate);
	return arenaTrack(r);
}
