"""Tests for NautilusFunction — first-class callable functions that are traced."""

import sys
import os

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "..", "build", "nautilus", "plugins", "python"))
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

from nautilus import Engine, ValInt32, ValInt64, ValFloat64, nautilus_function
import pytest


class TestNautilusFunctionCreation:
    """Test that NautilusFunction objects can be created via the decorator."""

    def test_create_i32(self):
        @nautilus_function("my_inc")
        def my_inc(x: ValInt32) -> ValInt32:
            return x + ValInt32(1)

        assert my_inc is not None

    def test_create_i64(self):
        @nautilus_function("double_i64")
        def double_it(x: ValInt64) -> ValInt64:
            return x + x

        assert double_it is not None

    def test_create_f64(self):
        @nautilus_function("negate_f64")
        def negate(x: ValFloat64) -> ValFloat64:
            return x * ValFloat64(-1.0)

        assert negate is not None

    def test_create_i32_binary(self):
        @nautilus_function("add_i32")
        def add(x: ValInt32, y: ValInt32) -> ValInt32:
            return x + y

        assert add is not None

    def test_infer_name_from_function(self):
        @nautilus_function()
        def my_func(x: ValInt32) -> ValInt32:
            return x

        assert my_func is not None

    def test_unsupported_signature_raises(self):
        with pytest.raises(TypeError):

            @nautilus_function("bad")
            def bad(x: int) -> int:
                return x


class TestNautilusFunctionInTracing:
    """Test NautilusFunction called from within a traced (compiled) function."""

    def test_call_from_compiled_i32(self):
        @nautilus_function("inc_nf")
        def inc_nf(x: ValInt32) -> ValInt32:
            return x + ValInt32(1)

        engine = Engine()

        @engine.compile
        def add_two(x: ValInt32) -> ValInt32:
            return inc_nf(inc_nf(x))

        assert add_two(10) == 12

    def test_call_from_compiled_i32_binary(self):
        @nautilus_function("mul_nf")
        def mul_nf(a: ValInt32, b: ValInt32) -> ValInt32:
            return a * b

        engine = Engine()

        @engine.compile
        def square(x: ValInt32) -> ValInt32:
            return mul_nf(x, x)

        assert square(7) == 49

    def test_call_from_compiled_f64(self):
        @nautilus_function("double_f64_nf")
        def double_it(x: ValFloat64) -> ValFloat64:
            return x * ValFloat64(2.0)

        engine = Engine()

        @engine.compile
        def quadruple(x: ValFloat64) -> ValFloat64:
            return double_it(double_it(x))

        result = quadruple(2.5)
        assert abs(result - 10.0) < 1e-10

    def test_two_nautilus_functions_in_one_compilation(self):
        @nautilus_function("add1")
        def add1(x: ValInt32) -> ValInt32:
            return x + ValInt32(1)

        @nautilus_function("times2")
        def times2(x: ValInt32) -> ValInt32:
            return x * ValInt32(2)

        engine = Engine()

        @engine.compile
        def combined(x: ValInt32) -> ValInt32:
            return times2(add1(x))

        assert combined(5) == 12  # (5+1)*2
