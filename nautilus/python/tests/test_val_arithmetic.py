"""Tests for val<T> arithmetic operations (non-traced, direct Python usage)."""

import sys
import os

# Add the build directory to path so nautilus_native can be found
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "..", "build", "nautilus", "python"))
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

from nautilus import ValInt32, ValInt64, ValFloat64, ValFloat32, ValBool


class TestValInt32Arithmetic:
    def test_add(self):
        a = ValInt32(10)
        b = ValInt32(3)
        c = a + b
        assert isinstance(c, ValInt32)

    def test_add_raw(self):
        a = ValInt32(10)
        c = a + 3
        assert isinstance(c, ValInt32)

    def test_radd(self):
        a = ValInt32(10)
        c = 5 + a
        assert isinstance(c, ValInt32)

    def test_sub(self):
        a = ValInt32(10)
        b = ValInt32(3)
        c = a - b
        assert isinstance(c, ValInt32)

    def test_mul(self):
        a = ValInt32(10)
        b = ValInt32(3)
        c = a * b
        assert isinstance(c, ValInt32)

    def test_truediv(self):
        a = ValInt32(10)
        b = ValInt32(3)
        c = a / b
        assert isinstance(c, ValInt32)

    def test_floordiv(self):
        a = ValInt32(10)
        b = ValInt32(3)
        c = a // b
        assert isinstance(c, ValInt32)

    def test_mod(self):
        a = ValInt32(10)
        b = ValInt32(3)
        c = a % b
        assert isinstance(c, ValInt32)

    def test_neg(self):
        a = ValInt32(10)
        c = -a
        assert isinstance(c, ValInt32)

    def test_iadd(self):
        a = ValInt32(10)
        a += ValInt32(5)
        assert isinstance(a, ValInt32)

    def test_iadd_raw(self):
        a = ValInt32(10)
        a += 5
        assert isinstance(a, ValInt32)

    def test_isub(self):
        a = ValInt32(10)
        a -= ValInt32(3)
        assert isinstance(a, ValInt32)

    def test_imul(self):
        a = ValInt32(10)
        a *= ValInt32(3)
        assert isinstance(a, ValInt32)


class TestValInt32Bitwise:
    def test_lshift(self):
        a = ValInt32(1)
        c = a << ValInt32(3)
        assert isinstance(c, ValInt32)

    def test_rshift(self):
        a = ValInt32(8)
        c = a >> ValInt32(2)
        assert isinstance(c, ValInt32)

    def test_and(self):
        a = ValInt32(0b1100)
        b = ValInt32(0b1010)
        c = a & b
        assert isinstance(c, ValInt32)

    def test_or(self):
        a = ValInt32(0b1100)
        b = ValInt32(0b1010)
        c = a | b
        assert isinstance(c, ValInt32)

    def test_xor(self):
        a = ValInt32(0b1100)
        b = ValInt32(0b1010)
        c = a ^ b
        assert isinstance(c, ValInt32)

    def test_invert(self):
        a = ValInt32(0)
        c = ~a
        assert isinstance(c, ValInt32)


class TestValInt32Comparison:
    def test_eq(self):
        a = ValInt32(10)
        b = ValInt32(10)
        c = a == b
        assert isinstance(c, ValBool)

    def test_ne(self):
        a = ValInt32(10)
        b = ValInt32(3)
        c = a != b
        assert isinstance(c, ValBool)

    def test_lt(self):
        a = ValInt32(3)
        b = ValInt32(10)
        c = a < b
        assert isinstance(c, ValBool)

    def test_le(self):
        a = ValInt32(3)
        b = ValInt32(10)
        c = a <= b
        assert isinstance(c, ValBool)

    def test_gt(self):
        a = ValInt32(10)
        b = ValInt32(3)
        c = a > b
        assert isinstance(c, ValBool)

    def test_ge(self):
        a = ValInt32(10)
        b = ValInt32(10)
        c = a >= b
        assert isinstance(c, ValBool)

    def test_eq_raw(self):
        a = ValInt32(10)
        c = a == 10
        assert isinstance(c, ValBool)


class TestValFloat64Arithmetic:
    def test_add(self):
        a = ValFloat64(3.14)
        b = ValFloat64(2.86)
        c = a + b
        assert isinstance(c, ValFloat64)

    def test_sub(self):
        a = ValFloat64(10.0)
        b = ValFloat64(3.5)
        c = a - b
        assert isinstance(c, ValFloat64)

    def test_mul(self):
        a = ValFloat64(2.0)
        b = ValFloat64(3.0)
        c = a * b
        assert isinstance(c, ValFloat64)

    def test_truediv(self):
        a = ValFloat64(10.0)
        b = ValFloat64(3.0)
        c = a / b
        assert isinstance(c, ValFloat64)

    def test_neg(self):
        a = ValFloat64(3.14)
        c = -a
        assert isinstance(c, ValFloat64)

    def test_add_raw(self):
        a = ValFloat64(3.14)
        c = a + 1.0
        assert isinstance(c, ValFloat64)

    def test_radd(self):
        a = ValFloat64(3.14)
        c = 1.0 + a
        assert isinstance(c, ValFloat64)


class TestValFloat64Comparison:
    def test_eq(self):
        a = ValFloat64(3.14)
        b = ValFloat64(3.14)
        c = a == b
        assert isinstance(c, ValBool)

    def test_lt(self):
        a = ValFloat64(1.0)
        b = ValFloat64(2.0)
        c = a < b
        assert isinstance(c, ValBool)


class TestValCasting:
    def test_int32_to_float64(self):
        a = ValInt32(42)
        b = a.cast_to_float64()
        assert isinstance(b, ValFloat64)

    def test_float64_to_int32(self):
        a = ValFloat64(3.14)
        b = a.cast_to_int32()
        assert isinstance(b, ValInt32)

    def test_int32_to_int64(self):
        a = ValInt32(42)
        b = a.cast_to_int64()
        assert isinstance(b, ValInt64)
