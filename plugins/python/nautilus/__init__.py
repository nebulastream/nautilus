"""Nautilus - Python bindings for the Nautilus tracing JIT compiler."""

from nautilus_native import (
    # Val types
    ValBool,
    ValInt32,
    ValInt64,
    ValFloat32,
    ValFloat64,
    ValObject,
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
    _register_i32_i32_i32_to_i32,
    _register_i64_to_i64,
    _register_i64_i64_to_i64,
    _register_i64_i64_i64_to_i64,
    _register_f64_to_f64,
    _register_f64_f64_to_f64,
    _register_f64_f64_f64_to_f64,
    _register_f32_to_f32,
    _register_f32_f32_to_f32,
    _register_f32_f32_f32_to_f32,
    _register_b_to_b,
    _register_i32_to_b,
    _register_obj_to_obj,
    _register_obj_obj_to_obj,
    _register_obj_obj_obj_to_obj,
)

# Module bindings
from nautilus_native import (
    _NautilusModule,
    _CompiledModule,
    _MF_i32_i32,
    _MF_i32_i32_i32,
    _MF_i32_i32_i32_i32,
    _MF_i64_i64,
    _MF_i64_i64_i64,
    _MF_i64_i64_i64_i64,
    _MF_f64_f64,
    _MF_f64_f64_f64,
    _MF_f64_f64_f64_f64,
    _MF_f32_f32,
    _MF_f32_f32_f32,
    _MF_f32_f32_f32_f32,
    _MF_b_b,
    _MF_b_i32,
    _MF_obj_obj,
    _MF_obj_obj_obj,
    _MF_obj_obj_obj_obj,
)

# NautilusFunction bindings
from nautilus_native import (
    _create_nautilus_func_i32_i32,
    _create_nautilus_func_i32_i32_i32,
    _create_nautilus_func_i32_i32_i32_i32,
    _create_nautilus_func_i64_i64,
    _create_nautilus_func_i64_i64_i64,
    _create_nautilus_func_i64_i64_i64_i64,
    _create_nautilus_func_f64_f64,
    _create_nautilus_func_f64_f64_f64,
    _create_nautilus_func_f64_f64_f64_f64,
    _create_nautilus_func_f32_f32,
    _create_nautilus_func_f32_f32_f32,
    _create_nautilus_func_f32_f32_f32_f32,
    _create_nautilus_func_b_b,
    _create_nautilus_func_b_i32,
    _create_nautilus_func_obj_obj,
    _create_nautilus_func_obj_obj_obj,
    _create_nautilus_func_obj_obj_obj_obj,
)

import inspect
import typing

# Python built-in type -> Val type mapping.
# Allows users to write `def f(x: int) -> int` instead of `def f(x: ValInt32) -> ValInt32`.
_PYTHON_TO_VAL = {
    int: ValInt32,
    float: ValFloat64,
    bool: ValBool,
    object: ValObject,
}


def _normalize_type(t):
    """Convert a Python built-in type to its Val equivalent, or return as-is."""
    return _PYTHON_TO_VAL.get(t, t)


def _normalize_signature(arg_types, ret_type):
    """Normalize a full signature, converting Python types to Val types."""
    return (
        tuple(_normalize_type(t) for t in arg_types),
        _normalize_type(ret_type) if ret_type is not None else None,
    )


# Type aliases for explicit bit-width control in annotations.
# Usage: `def f(x: nautilus.int64) -> nautilus.int64: ...`
int32 = ValInt32
int64 = ValInt64
float32 = ValFloat32
float64 = ValFloat64

# Signature registry: (arg_types_tuple, return_type) -> registration function
_REGISTRY = {
    ((ValInt32,), ValInt32): _register_i32_to_i32,
    ((ValInt32, ValInt32), ValInt32): _register_i32_i32_to_i32,
    ((ValInt32, ValInt32, ValInt32), ValInt32): _register_i32_i32_i32_to_i32,
    ((ValInt64,), ValInt64): _register_i64_to_i64,
    ((ValInt64, ValInt64), ValInt64): _register_i64_i64_to_i64,
    ((ValInt64, ValInt64, ValInt64), ValInt64): _register_i64_i64_i64_to_i64,
    ((ValFloat64,), ValFloat64): _register_f64_to_f64,
    ((ValFloat64, ValFloat64), ValFloat64): _register_f64_f64_to_f64,
    ((ValFloat64, ValFloat64, ValFloat64), ValFloat64): _register_f64_f64_f64_to_f64,
    ((ValFloat32,), ValFloat32): _register_f32_to_f32,
    ((ValFloat32, ValFloat32), ValFloat32): _register_f32_f32_to_f32,
    ((ValFloat32, ValFloat32, ValFloat32), ValFloat32): _register_f32_f32_f32_to_f32,
    ((ValBool,), ValBool): _register_b_to_b,
    ((ValInt32,), ValBool): _register_i32_to_b,
    ((ValObject,), ValObject): _register_obj_to_obj,
    ((ValObject, ValObject), ValObject): _register_obj_obj_to_obj,
    ((ValObject, ValObject, ValObject), ValObject): _register_obj_obj_obj_to_obj,
}

# Module register/get method lookup: (arg_types, ret_type) -> (register_method_name, get_method_name)
_MODULE_REGISTRY = {
    ((ValInt32,), ValInt32): ("register_i32_to_i32", "get_i32_i32"),
    ((ValInt32, ValInt32), ValInt32): ("register_i32_i32_to_i32", "get_i32_i32_i32"),
    ((ValInt32, ValInt32, ValInt32), ValInt32): ("register_i32_i32_i32_to_i32", "get_i32_i32_i32_i32"),
    ((ValInt64,), ValInt64): ("register_i64_to_i64", "get_i64_i64"),
    ((ValInt64, ValInt64), ValInt64): ("register_i64_i64_to_i64", "get_i64_i64_i64"),
    ((ValInt64, ValInt64, ValInt64), ValInt64): ("register_i64_i64_i64_to_i64", "get_i64_i64_i64_i64"),
    ((ValFloat64,), ValFloat64): ("register_f64_to_f64", "get_f64_f64"),
    ((ValFloat64, ValFloat64), ValFloat64): ("register_f64_f64_to_f64", "get_f64_f64_f64"),
    ((ValFloat64, ValFloat64, ValFloat64), ValFloat64): ("register_f64_f64_f64_to_f64", "get_f64_f64_f64_f64"),
    ((ValFloat32,), ValFloat32): ("register_f32_to_f32", "get_f32_f32"),
    ((ValFloat32, ValFloat32), ValFloat32): ("register_f32_f32_to_f32", "get_f32_f32_f32"),
    ((ValFloat32, ValFloat32, ValFloat32), ValFloat32): ("register_f32_f32_f32_to_f32", "get_f32_f32_f32_f32"),
    ((ValBool,), ValBool): ("register_b_to_b", "get_b_b"),
    ((ValInt32,), ValBool): ("register_i32_to_b", "get_b_i32"),
    ((ValObject,), ValObject): ("register_obj_to_obj", "get_obj_obj"),
    ((ValObject, ValObject), ValObject): ("register_obj_obj_to_obj", "get_obj_obj_obj"),
    ((ValObject, ValObject, ValObject), ValObject): ("register_obj_obj_obj_to_obj", "get_obj_obj_obj_obj"),
}

# NautilusFunction factory lookup: (arg_types, ret_type) -> create_fn
_NAUTILUS_FUNC_REGISTRY = {
    ((ValInt32,), ValInt32): _create_nautilus_func_i32_i32,
    ((ValInt32, ValInt32), ValInt32): _create_nautilus_func_i32_i32_i32,
    ((ValInt32, ValInt32, ValInt32), ValInt32): _create_nautilus_func_i32_i32_i32_i32,
    ((ValInt64,), ValInt64): _create_nautilus_func_i64_i64,
    ((ValInt64, ValInt64), ValInt64): _create_nautilus_func_i64_i64_i64,
    ((ValInt64, ValInt64, ValInt64), ValInt64): _create_nautilus_func_i64_i64_i64_i64,
    ((ValFloat64,), ValFloat64): _create_nautilus_func_f64_f64,
    ((ValFloat64, ValFloat64), ValFloat64): _create_nautilus_func_f64_f64_f64,
    ((ValFloat64, ValFloat64, ValFloat64), ValFloat64): _create_nautilus_func_f64_f64_f64_f64,
    ((ValFloat32,), ValFloat32): _create_nautilus_func_f32_f32,
    ((ValFloat32, ValFloat32), ValFloat32): _create_nautilus_func_f32_f32_f32,
    ((ValFloat32, ValFloat32, ValFloat32), ValFloat32): _create_nautilus_func_f32_f32_f32_f32,
    ((ValBool,), ValBool): _create_nautilus_func_b_b,
    ((ValInt32,), ValBool): _create_nautilus_func_b_i32,
    ((ValObject,), ValObject): _create_nautilus_func_obj_obj,
    ((ValObject, ValObject), ValObject): _create_nautilus_func_obj_obj_obj,
    ((ValObject, ValObject, ValObject), ValObject): _create_nautilus_func_obj_obj_obj_obj,
}


def _get_signature(func):
    """Extract (arg_types, return_type) from function type annotations.

    Python built-in types (int, float, bool) are automatically mapped
    to their Val equivalents (ValInt32, ValFloat64, ValBool).
    """
    hints = typing.get_type_hints(func)
    params = inspect.signature(func).parameters
    arg_types = tuple(hints[name] for name in params if name in hints)
    ret_type = hints.get("return")
    return _normalize_signature(arg_types, ret_type)


class Engine:
    """High-level Python wrapper around NautilusEngine with decorator support.

    Usage:
        engine = Engine(backend="mlir")

        @engine.compile
        def add(x: int, y: int) -> int:
            return x + y

        result = add(5, 3)  # returns 8

    Compiler options can be passed as keyword arguments using dotted names
    or with dots replaced by underscores:

        engine = Engine(backend="mlir", dump_console=True)
        engine = Engine(backend="cpp", mlir_optimizationLevel=2)

    Or via an Options object:

        opts = Options()
        opts.set("dump.console", True)
        engine = Engine(options=opts)

    See docs/options.md for the full list of available options.
    """

    def __init__(self, backend="mlir", options=None, **kwargs):
        if options is not None:
            options.set("engine.backend", backend)
            self._engine = NautilusEngine(options)
        elif kwargs:
            opts = Options()
            opts.set("engine.backend", backend)
            for key, value in kwargs.items():
                # Allow underscores as dot separators: dump_console -> dump.console
                opt_key = key.replace("_", ".")
                opts.set(opt_key, value)
            self._engine = NautilusEngine(opts)
        else:
            self._engine = NautilusEngine(backend)

    @property
    def is_compiled(self):
        return self._engine.is_compiled()

    @property
    def backend_name(self):
        return self._engine.backend_name()

    def register(self, func):
        """Register and compile a Python function for JIT execution.

        The function must have type annotations using Python types (int, float, bool)
        or Val types (ValInt32, ValFloat64, etc.).
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
            def my_func(x: int) -> int:
                return x + 1
        """
        return self.register(func)

    def create_module(self):
        """Create a Module for batch-compiling multiple named functions.

        Usage:
            module = engine.create_module()
            module.register("inc", lambda x: x + ValInt32(1),
                            arg_types=(ValInt32,), ret_type=ValInt32)
            compiled = module.compile()
            inc = compiled.get("inc", arg_types=(ValInt32,), ret_type=ValInt32)
            assert inc(5) == 6
        """
        return Module(self._engine.create_module())


class Module:
    """Batch-register multiple named functions and compile them together.

    Functions can be registered either with explicit type annotations or
    with explicit arg_types/ret_type keyword arguments.
    """

    def __init__(self, native_module):
        self._module = native_module
        self._signatures = {}  # name -> (arg_types, ret_type)

    def register(self, name, func, *, arg_types=None, ret_type=None):
        """Register a named function in the module.

        Args:
            name: Unique name for this function.
            func: The Python callable.
            arg_types: Tuple of Val types for arguments (inferred from annotations if None).
            ret_type: Val type for return value (inferred from annotations if None).
        """
        if arg_types is None or ret_type is None:
            inferred_args, inferred_ret = _get_signature(func)
            if arg_types is None:
                arg_types = inferred_args
            if ret_type is None:
                ret_type = inferred_ret

        arg_types, ret_type = _normalize_signature(arg_types, ret_type)
        key = (arg_types, ret_type)
        entry = _MODULE_REGISTRY.get(key)
        if entry is None:
            supported = ", ".join(
                f"({', '.join(t.__name__ for t in args)}) -> {ret.__name__}"
                for (args, ret) in _MODULE_REGISTRY.keys()
            )
            raise TypeError(
                f"Unsupported module function signature: "
                f"({', '.join(t.__name__ for t in arg_types)}) -> "
                f"{ret_type.__name__ if ret_type else 'None'}. "
                f"Supported: {supported}"
            )

        register_method_name = entry[0]
        getattr(self._module, register_method_name)(name, func)
        self._signatures[name] = key

    def compile(self):
        """Compile all registered functions and return a CompiledModule."""
        native_compiled = self._module.compile()
        return CompiledModule(native_compiled, self._signatures)


class CompiledModule:
    """Result of compiling a Module. Provides typed function access by name."""

    def __init__(self, native_compiled, signatures):
        self._compiled = native_compiled
        self._signatures = signatures

    def get(self, name, *, arg_types=None, ret_type=None):
        """Get a callable function handle by name.

        If the function was registered via Module.register(), the signature is
        remembered and arg_types/ret_type can be omitted.
        """
        if arg_types is None or ret_type is None:
            if name in self._signatures:
                saved_args, saved_ret = self._signatures[name]
                if arg_types is None:
                    arg_types = saved_args
                if ret_type is None:
                    ret_type = saved_ret

        if arg_types is None or ret_type is None:
            raise ValueError(
                f"Cannot determine signature for '{name}'. "
                f"Provide arg_types and ret_type explicitly."
            )

        arg_types, ret_type = _normalize_signature(arg_types, ret_type)
        key = (arg_types, ret_type)
        entry = _MODULE_REGISTRY.get(key)
        if entry is None:
            raise TypeError(f"Unsupported signature for module function '{name}'.")

        get_method_name = entry[1]
        return getattr(self._compiled, get_method_name)(name)


def nautilus_function(name=None, *, arg_types=None, ret_type=None):
    """Decorator to create a NautilusFunction from a Python callable.

    A NautilusFunction can be called from within other traced functions —
    the call is intercepted during tracing and compiled as a separate function.

    Usage:
        @nautilus_function("double_it")
        def double_it(x: int) -> int:
            return x + x

        # Can be called from another traced function:
        @engine.compile
        def quadruple(x: int) -> int:
            return double_it(double_it(x))
    """
    def decorator(func):
        fn_name = name if name is not None else func.__name__

        if arg_types is not None and ret_type is not None:
            args = arg_types
            ret = ret_type
        else:
            inferred_args, inferred_ret = _get_signature(func)
            args = arg_types if arg_types is not None else inferred_args
            ret = ret_type if ret_type is not None else inferred_ret

        args, ret = _normalize_signature(args, ret)
        key = (args, ret)
        create_fn = _NAUTILUS_FUNC_REGISTRY.get(key)
        if create_fn is None:
            supported = ", ".join(
                f"({', '.join(t.__name__ for t in a)}) -> {r.__name__}"
                for (a, r) in _NAUTILUS_FUNC_REGISTRY.keys()
            )
            raise TypeError(
                f"Unsupported NautilusFunction signature: "
                f"({', '.join(t.__name__ for t in args)}) -> "
                f"{ret.__name__ if ret else 'None'}. "
                f"Supported: {supported}"
            )

        return create_fn(fn_name, func)

    return decorator


__all__ = [
    "ValBool",
    "ValInt32",
    "ValInt64",
    "ValFloat32",
    "ValFloat64",
    "ValObject",
    "Engine",
    "Module",
    "CompiledModule",
    "NautilusEngine",
    "Options",
    "select",
    "nautilus_function",
    "int32",
    "int64",
    "float32",
    "float64",
]
