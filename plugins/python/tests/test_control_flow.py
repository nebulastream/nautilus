"""Tests for nested if/else and while-loop control flow through tracing."""

from nautilus import Engine, ValInt32


class TestNestedIf:
    def test_nested_if_true_true(self):
        engine = Engine()

        @engine.compile
        def f(x: ValInt32) -> ValInt32:
            result = ValInt32(0)
            if x > ValInt32(5):
                if x > ValInt32(10):
                    result = x + ValInt32(100)
                else:
                    result = x + ValInt32(50)
            else:
                result = ValInt32(-1)
            return result

        assert f(15) == 115

    def test_nested_if_true_false(self):
        engine = Engine()

        @engine.compile
        def f(x: ValInt32) -> ValInt32:
            result = ValInt32(0)
            if x > ValInt32(5):
                if x > ValInt32(10):
                    result = x + ValInt32(100)
                else:
                    result = x + ValInt32(50)
            else:
                result = ValInt32(-1)
            return result

        assert f(7) == 57

    def test_nested_if_false(self):
        engine = Engine()

        @engine.compile
        def f(x: ValInt32) -> ValInt32:
            result = ValInt32(0)
            if x > ValInt32(5):
                if x > ValInt32(10):
                    result = x + ValInt32(100)
                else:
                    result = x + ValInt32(50)
            else:
                result = ValInt32(-1)
            return result

        assert f(3) == -1

    def test_nested_if_all_paths(self):
        """Test all three paths of a nested if in a single compiled function."""
        engine = Engine()

        @engine.compile
        def f(x: ValInt32) -> ValInt32:
            result = ValInt32(0)
            if x > ValInt32(5):
                if x > ValInt32(10):
                    result = x + ValInt32(100)
                else:
                    result = x + ValInt32(50)
            else:
                result = ValInt32(-1)
            return result

        assert f(15) == 115
        assert f(7) == 57
        assert f(3) == -1
        assert f(0) == -1
        assert f(20) == 120


class TestCodeAfterIf:
    def test_code_after_simple_if(self):
        """Code after an if/else block triggers control-flow merge."""
        engine = Engine()

        @engine.compile
        def f(x: ValInt32) -> ValInt32:
            result = ValInt32(0)
            if x > ValInt32(0):
                result = x
            else:
                result = -x
            return result + ValInt32(1)

        assert f(5) == 6
        assert f(-5) == 6
        assert f(0) == 1


class TestWhileLoop:
    def test_sum_to_n(self):
        engine = Engine()

        @engine.compile
        def sum_to(n: ValInt32) -> ValInt32:
            total = ValInt32(0)
            i = ValInt32(1)
            while i <= n:
                total += i
                i += ValInt32(1)
            return total

        assert sum_to(10) == 55

    def test_while_zero_iterations(self):
        engine = Engine()

        @engine.compile
        def sum_to(n: ValInt32) -> ValInt32:
            total = ValInt32(0)
            i = ValInt32(1)
            while i <= n:
                total += i
                i += ValInt32(1)
            return total

        assert sum_to(0) == 0

    def test_countdown(self):
        engine = Engine()

        @engine.compile
        def countdown(n: ValInt32) -> ValInt32:
            result = ValInt32(0)
            i = n
            while i > ValInt32(0):
                result += i
                i -= ValInt32(1)
            return result

        assert countdown(5) == 15
