"""Nautilus - Python bindings for the Nautilus tracing JIT compiler."""

from nautilus_native import (
    # Val types
    ValBool,
    ValInt32,
    ValInt64,
    ValFloat32,
    ValFloat64,
    # Engine
    NautilusEngine,
    Options,
    # Operations
    select,
)

# Registration function lookup table: maps (arg_types, ret_type) -> C++ register fn
from nautilus_native import (
    _register_i32_to_i32,
    _register_i32_i32_to_i32,
    _register_i64_to_i64,
    _register_i64_i64_to_i64,
    _register_f64_to_f64,
    _register_f64_f64_to_f64,
    _register_f32_to_f32,
    _register_b_to_b,
    _register_i32_to_b,
)

import inspect
import typing

# Signature registry: (arg_types_tuple, return_type) -> registration function
_REGISTRY = {
    ((ValInt32,), ValInt32): _register_i32_to_i32,
    ((ValInt32, ValInt32), ValInt32): _register_i32_i32_to_i32,
    ((ValInt64,), ValInt64): _register_i64_to_i64,
    ((ValInt64, ValInt64), ValInt64): _register_i64_i64_to_i64,
    ((ValFloat64,), ValFloat64): _register_f64_to_f64,
    ((ValFloat64, ValFloat64), ValFloat64): _register_f64_f64_to_f64,
    ((ValFloat32,), ValFloat32): _register_f32_to_f32,
    ((ValBool,), ValBool): _register_b_to_b,
    ((ValInt32,), ValBool): _register_i32_to_b,
}


def _get_signature(func):
    """Extract (arg_types, return_type) from function type annotations."""
    hints = typing.get_type_hints(func)
    params = inspect.signature(func).parameters
    arg_types = tuple(hints[name] for name in params if name in hints)
    ret_type = hints.get("return")
    return arg_types, ret_type


class Engine:
    """High-level Python wrapper around NautilusEngine with decorator support.

    Usage:
        engine = Engine(backend="mlir")

        @engine.compile
        def add(x: ValInt32, y: ValInt32) -> ValInt32:
            return x + y

        result = add(5, 3)  # returns 8
    """

    def __init__(self, backend="mlir"):
        self._engine = NautilusEngine(backend)

    @property
    def is_compiled(self):
        return self._engine.is_compiled()

    @property
    def backend_name(self):
        return self._engine.backend_name()

    def register(self, func):
        """Register and compile a Python function for JIT execution.

        The function must have type annotations using Val types.
        Returns a callable that accepts raw Python values.
        """
        arg_types, ret_type = _get_signature(func)
        key = (arg_types, ret_type)
        register_fn = _REGISTRY.get(key)
        if register_fn is None:
            supported = ", ".join(
                f"({', '.join(t.__name__ for t in args)}) -> {ret.__name__}"
                for (args, ret) in _REGISTRY.keys()
            )
            raise TypeError(
                f"Unsupported function signature: "
                f"({', '.join(t.__name__ for t in arg_types)}) -> "
                f"{ret_type.__name__ if ret_type else 'None'}. "
                f"Supported signatures: {supported}"
            )
        return register_fn(self._engine, func)

    def compile(self, func):
        """Decorator: register and compile a function.

        Usage:
            @engine.compile
            def my_func(x: ValInt32) -> ValInt32:
                return x + ValInt32(1)
        """
        return self.register(func)


__all__ = [
    "ValBool",
    "ValInt32",
    "ValInt64",
    "ValFloat32",
    "ValFloat64",
    "Engine",
    "NautilusEngine",
    "Options",
    "select",
]
