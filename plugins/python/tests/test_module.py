"""Tests for NautilusModule batch compilation and multiple functions on the same engine."""

import sys
import os

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "..", "build", "nautilus", "plugins", "python"))
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

from nautilus import Engine, Module, ValInt32, ValInt64, ValFloat64
import pytest


class TestMultipleFunctionsOnSameEngine:
    """Test that an Engine can compile multiple independent functions."""

    def test_two_i32_functions(self):
        engine = Engine()

        @engine.compile
        def inc(x: ValInt32) -> ValInt32:
            return x + ValInt32(1)

        @engine.compile
        def dec(x: ValInt32) -> ValInt32:
            return x - ValInt32(1)

        assert inc(10) == 11
        assert dec(10) == 9

    def test_mixed_types(self):
        engine = Engine()

        @engine.compile
        def add_i32(x: ValInt32, y: ValInt32) -> ValInt32:
            return x + y

        @engine.compile
        def add_f64(x: ValFloat64, y: ValFloat64) -> ValFloat64:
            return x + y

        assert add_i32(3, 4) == 7
        assert abs(add_f64(1.5, 2.5) - 4.0) < 1e-10

    def test_many_functions(self):
        engine = Engine()

        @engine.compile
        def f1(x: ValInt32) -> ValInt32:
            return x + ValInt32(1)

        @engine.compile
        def f2(x: ValInt32) -> ValInt32:
            return x + ValInt32(2)

        @engine.compile
        def f3(x: ValInt32) -> ValInt32:
            return x + ValInt32(3)

        assert f1(0) == 1
        assert f2(0) == 2
        assert f3(0) == 3


class TestModule:
    """Test NautilusModule for batch compilation."""

    def test_single_function(self):
        engine = Engine()
        module = engine.create_module()
        module.register(
            "inc",
            lambda x: x + ValInt32(1),
            arg_types=(ValInt32,),
            ret_type=ValInt32,
        )
        compiled = module.compile()
        inc = compiled.get("inc")
        assert inc(10) == 11

    def test_two_functions_same_type(self):
        engine = Engine()
        module = engine.create_module()
        module.register(
            "inc",
            lambda x: x + ValInt32(1),
            arg_types=(ValInt32,),
            ret_type=ValInt32,
        )
        module.register(
            "double",
            lambda x: x * ValInt32(2),
            arg_types=(ValInt32,),
            ret_type=ValInt32,
        )
        compiled = module.compile()
        inc = compiled.get("inc")
        double = compiled.get("double")
        assert inc(5) == 6
        assert double(5) == 10

    def test_mixed_types_in_module(self):
        engine = Engine()
        module = engine.create_module()
        module.register(
            "add_i32",
            lambda x, y: x + y,
            arg_types=(ValInt32, ValInt32),
            ret_type=ValInt32,
        )
        module.register(
            "mul_i64",
            lambda a, b: a * b,
            arg_types=(ValInt64, ValInt64),
            ret_type=ValInt64,
        )
        compiled = module.compile()
        add = compiled.get("add_i32")
        mul = compiled.get("mul_i64")
        assert add(3, 4) == 7
        assert mul(6, 7) == 42

    def test_annotated_function_in_module(self):
        engine = Engine()
        module = engine.create_module()

        def my_inc(x: ValInt32) -> ValInt32:
            return x + ValInt32(1)

        module.register("inc", my_inc)
        compiled = module.compile()
        inc = compiled.get("inc")
        assert inc(99) == 100

    def test_f64_function_in_module(self):
        engine = Engine()
        module = engine.create_module()
        module.register(
            "half",
            lambda x: x * ValFloat64(0.5),
            arg_types=(ValFloat64,),
            ret_type=ValFloat64,
        )
        compiled = module.compile()
        half = compiled.get("half")
        assert abs(half(10.0) - 5.0) < 1e-10

    def test_unsupported_signature_raises(self):
        engine = Engine()
        module = engine.create_module()
        with pytest.raises(TypeError):
            module.register(
                "bad",
                lambda x: x,
                arg_types=(int,),
                ret_type=int,
            )
