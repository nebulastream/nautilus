#pragma once
#include <Python.h>
#include <vector>

/// Thread-local arena for tracking new PyObject* references created by
/// trampolines during compiled execution.  After the compiled function
/// returns, the caller borrows the result and clears the arena.
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
