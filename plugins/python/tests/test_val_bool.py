"""Tests for val<bool> operations."""

from nautilus import ValBool


class TestValBoolConstruction:
    def test_default(self):
        b = ValBool()
        assert isinstance(b, ValBool)

    def test_true(self):
        b = ValBool(True)
        assert isinstance(b, ValBool)

    def test_false(self):
        b = ValBool(False)
        assert isinstance(b, ValBool)


class TestValBoolConversion:
    def test_bool_true(self):
        b = ValBool(True)
        assert bool(b) is True

    def test_bool_false(self):
        b = ValBool(False)
        assert bool(b) is False


class TestValBoolLogical:
    def test_and(self):
        a = ValBool(True)
        b = ValBool(False)
        c = a & b
        assert isinstance(c, ValBool)

    def test_or(self):
        a = ValBool(True)
        b = ValBool(False)
        c = a | b
        assert isinstance(c, ValBool)

    def test_invert(self):
        a = ValBool(True)
        c = ~a
        assert isinstance(c, ValBool)


class TestValBoolComparison:
    def test_eq(self):
        a = ValBool(True)
        b = ValBool(True)
        c = a == b
        assert isinstance(c, ValBool)

    def test_ne(self):
        a = ValBool(True)
        b = ValBool(False)
        c = a != b
        assert isinstance(c, ValBool)

    def test_eq_raw(self):
        a = ValBool(True)
        c = a == True  # noqa: E712
        assert isinstance(c, ValBool)


class TestValBoolProbability:
    def test_default_probability(self):
        b = ValBool(True)
        assert b.probability == 0.5

    def test_set_probability(self):
        b = ValBool(True)
        b.set_probability(0.9)
        assert b.probability == 0.9
