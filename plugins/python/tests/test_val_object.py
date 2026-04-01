"""Tests for ValObject — generic Python object support via val<void*>."""

import sys
import os

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "..", "build", "nautilus", "plugins", "python"))
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

from nautilus import Engine, ValObject
import pytest


class TestValObjectBasic:
    """Test ValObject creation and basic operations outside tracing."""

    def test_create_from_int(self):
        v = ValObject(42)
        assert v is not None

    def test_create_from_float(self):
        v = ValObject(3.14)
        assert v is not None

    def test_create_from_string(self):
        v = ValObject("hello")
        assert v is not None

    def test_repr(self):
        v = ValObject(42)
        assert repr(v) == "ValObject(...)"


class TestValObjectCompileArithmetic:
    """Test compiled functions using generic object types."""

    def test_add_ints(self):
        engine = Engine()

        @engine.compile
        def add(x: object, y: object) -> object:
            return x + y

        assert add(5, 3) == 8

    def test_add_floats(self):
        engine = Engine()

        @engine.compile
        def add(x: object, y: object) -> object:
            return x + y

        result = add(3.14, 2.86)
        assert abs(result - 6.0) < 1e-10

    def test_add_strings(self):
        engine = Engine()

        @engine.compile
        def concat(x: object, y: object) -> object:
            return x + y

        assert concat("hello", " world") == "hello world"

    def test_subtract(self):
        engine = Engine()

        @engine.compile
        def sub(x: object, y: object) -> object:
            return x - y

        assert sub(10, 3) == 7

    def test_multiply(self):
        engine = Engine()

        @engine.compile
        def mul(x: object, y: object) -> object:
            return x * y

        assert mul(6, 7) == 42

    def test_multiply_string(self):
        engine = Engine()

        @engine.compile
        def repeat(x: object, y: object) -> object:
            return x * y

        assert repeat("ab", 3) == "ababab"

    def test_negate(self):
        engine = Engine()

        @engine.compile
        def neg(x: object) -> object:
            return -x

        assert neg(42) == -42

    def test_add_with_constant(self):
        engine = Engine()

        @engine.compile
        def add_ten(x: object) -> object:
            return x + 10

        assert add_ten(5) == 15


class TestValObjectCompileUnary:
    """Test unary object functions."""

    def test_identity(self):
        engine = Engine()

        @engine.compile
        def identity(x: object) -> object:
            return x

        assert identity(42) == 42
        assert identity("hello") == "hello"

    def test_negate_float(self):
        engine = Engine()

        @engine.compile
        def neg(x: object) -> object:
            return -x

        assert abs(neg(3.14) - (-3.14)) < 1e-10
