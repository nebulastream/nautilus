"""Tests for Engine compilation and execution."""

import sys
import os

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "..", "build", "nautilus", "plugins", "python"))
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

from nautilus import Engine, ValInt32, ValInt64, ValFloat64, ValBool, select
import pytest


class TestEngineCreation:
    def test_default_engine(self):
        engine = Engine()
        assert engine.is_compiled

    def test_engine_backend_name(self):
        engine = Engine(backend="mlir")
        name = engine.backend_name
        assert isinstance(name, str)


class TestEngineCompileInt32:
    def test_identity(self):
        engine = Engine()

        @engine.compile
        def identity(x: ValInt32) -> ValInt32:
            return x

        assert identity(42) == 42

    def test_add(self):
        engine = Engine()

        @engine.compile
        def add(x: ValInt32, y: ValInt32) -> ValInt32:
            return x + y

        assert add(5, 3) == 8

    def test_arithmetic(self):
        engine = Engine()

        @engine.compile
        def compute(x: ValInt32, y: ValInt32) -> ValInt32:
            return x * y + x

        assert compute(3, 4) == 15  # 3*4 + 3

    def test_constant(self):
        engine = Engine()

        @engine.compile
        def add_ten(x: ValInt32) -> ValInt32:
            return x + ValInt32(10)

        assert add_ten(5) == 15


class TestEngineCompileFloat64:
    def test_add(self):
        engine = Engine()

        @engine.compile
        def add(x: ValFloat64, y: ValFloat64) -> ValFloat64:
            return x + y

        result = add(3.14, 2.86)
        assert abs(result - 6.0) < 1e-10

    def test_mul(self):
        engine = Engine()

        @engine.compile
        def mul(x: ValFloat64) -> ValFloat64:
            return x * ValFloat64(2.0)

        result = mul(3.5)
        assert abs(result - 7.0) < 1e-10


class TestEngineControlFlow:
    def test_if_else(self):
        engine = Engine()

        @engine.compile
        def abs_val(x: ValInt32) -> ValInt32:
            result = ValInt32(0)
            if x >= ValInt32(0):
                result = x
            else:
                result = -x
            return result

        assert abs_val(5) == 5
        assert abs_val(-5) == 5
        assert abs_val(0) == 0


class TestSelect:
    def test_select_int32(self):
        cond = ValBool(True)
        a = ValInt32(10)
        b = ValInt32(20)
        result = select(cond, a, b)
        assert isinstance(result, ValInt32)

    def test_select_float64(self):
        cond = ValBool(False)
        a = ValFloat64(1.0)
        b = ValFloat64(2.0)
        result = select(cond, a, b)
        assert isinstance(result, ValFloat64)


class TestUnsupportedSignature:
    def test_raises_type_error(self):
        engine = Engine()
        with pytest.raises(TypeError):

            @engine.compile
            def bad_func(x: int) -> int:
                return x
