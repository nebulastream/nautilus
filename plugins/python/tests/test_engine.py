"""Tests for Engine compilation and execution."""

from nautilus import Engine, ValInt32, ValInt64, ValFloat64, ValBool, select, Options
import nautilus
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


class TestEngineCompilePythonTypes:
    def test_int_identity(self):
        engine = Engine()

        @engine.compile
        def identity(x: int) -> int:
            return x

        assert identity(42) == 42

    def test_int_add(self):
        engine = Engine()

        @engine.compile
        def add(x: int, y: int) -> int:
            return x + y

        assert add(5, 3) == 8

    def test_int_constant(self):
        engine = Engine()

        @engine.compile
        def add_ten(x: int) -> int:
            return x + 10

        assert add_ten(5) == 15

    def test_float_add(self):
        engine = Engine()

        @engine.compile
        def add(x: float, y: float) -> float:
            return x + y

        assert abs(add(3.14, 2.86) - 6.0) < 1e-10

    def test_float_mul(self):
        engine = Engine()

        @engine.compile
        def mul(x: float) -> float:
            return x * 2.0

        assert abs(mul(3.5) - 7.0) < 1e-10

    def test_if_else_with_python_types(self):
        engine = Engine()

        @engine.compile
        def abs_val(x: int) -> int:
            result = ValInt32(0)
            if x >= 0:
                result = x
            else:
                result = -x
            return result

        assert abs_val(5) == 5
        assert abs_val(-5) == 5

    def test_int64_alias(self):
        engine = Engine()

        @engine.compile
        def add_i64(x: nautilus.int64, y: nautilus.int64) -> nautilus.int64:
            return x + y

        assert add_i64(100, 200) == 300

    def test_float32_alias(self):
        engine = Engine()

        @engine.compile
        def identity_f32(x: nautilus.float32) -> nautilus.float32:
            return x

        result = identity_f32(3.14)
        assert abs(result - 3.14) < 0.01


class TestEngineOptions:
    """Test that compiler options can be passed to Engine."""

    def test_kwargs_compile(self):
        """Options via kwargs still produce working compiled functions."""
        engine = Engine(backend="mlir", mlir_optimizationLevel=2)

        @engine.compile
        def add(x: int, y: int) -> int:
            return x + y

        assert add(3, 4) == 7

    def test_options_object(self):
        """Options via an Options object work correctly."""
        opts = Options()
        opts.set("mlir.optimizationLevel", 2)
        engine = Engine(options=opts)

        @engine.compile
        def double(x: int) -> int:
            return x + x

        assert double(21) == 42

    def test_options_set_types(self):
        """Options.set() auto-detects bool, int, float, str."""
        opts = Options()
        opts.set("engine.backend", "mlir")
        opts.set("engine.compilation", True)
        opts.set("mlir.optimizationLevel", 3)
        # Should not raise
        engine = Engine(options=opts)
        assert engine.is_compiled

    def test_options_set_rejects_invalid_type(self):
        """Options.set() raises TypeError for unsupported value types."""
        opts = Options()
        with pytest.raises(TypeError):
            opts.set("key", [1, 2, 3])

    def test_dump_option_does_not_break_compilation(self):
        """Dump options are accepted without breaking compilation."""
        engine = Engine(dump_console=False, dump_after_tracing=False)

        @engine.compile
        def inc(x: int) -> int:
            return x + ValInt32(1)

        assert inc(10) == 11


class TestEngineCompileThreeArgs:
    def test_int_three_args(self):
        engine = Engine()

        @engine.compile
        def weighted_sum(a: int, b: int, c: int) -> int:
            return a * ValInt32(1) + b * ValInt32(2) + c * ValInt32(3)

        assert weighted_sum(1, 2, 3) == 14  # 1*1 + 2*2 + 3*3

    def test_float_three_args(self):
        engine = Engine()

        @engine.compile
        def fma(a: float, b: float, c: float) -> float:
            return a * b + c

        result = fma(2.0, 3.0, 1.0)
        assert abs(result - 7.0) < 1e-10

    def test_int_three_args_arithmetic(self):
        engine = Engine()

        @engine.compile
        def sum3(a: int, b: int, c: int) -> int:
            return a + b + c

        assert sum3(1, 2, 3) == 6


class TestUnsupportedSignature:
    def test_raises_type_error(self):
        engine = Engine()
        with pytest.raises(TypeError):

            @engine.compile
            def bad_func(x: str) -> str:
                return x
