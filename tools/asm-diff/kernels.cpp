// asm-diff kernels (tools/asm-diff): loops that run long enough for the quality of the generated code to show, in the
// shapes that matter: counted loops, array walks at every element size, nested loops, selects, narrow and float
// arithmetic, internal and external calls.
//
//   nautilus-asm-kernels run [reps [kernel]]   steady-state microseconds per call: the NIR and the 0.3 lowering are
//                                              compiled first and then timed in alternation (best of reps each), so
//                                              that their ratio does not depend on code placement or on the machine's
//                                              mood; pin the process to one core
//   nautilus-asm-kernels dump [kernel]         the assembly of every kernel on both pipelines, between "== pipeline
//                                              kernel" lines (tools/asm-diff/loop-diff.py counts the instructions of
//                                              the loops)
//
// Same source on every branch, so it also runs against `main`.
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include "nautilus/function.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/select.hpp"
#include "nautilus/static.hpp"
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <memory>
#include <string>
#include <vector>
using namespace nautilus;
using Clock = std::chrono::steady_clock;

val<int32_t> fib(val<int32_t> n) {
	val<int32_t> a = 0, b = 1, c;
	for (val<int> i = 2; i <= n; i = i + 1) {
		c = a + b;
		a = b;
		b = c;
	}
	return b;
}
val<int64_t> callCalleeBody(val<int64_t> x, val<int64_t> y) {
	return x * 2 + y;
}
static auto callBenchCallee = NautilusFunction {"callBenchCallee", callCalleeBody};
val<int64_t> internalCallLoop(val<int32_t> n) {
	val<int64_t> acc = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		acc = callBenchCallee(acc, val<int64_t>(1));
	}
	return acc;
}
int64_t nativeAdd(int64_t a, int64_t b) noexcept {
	return a + b;
}
val<int64_t> externalCallLoop(val<int32_t> n) {
	val<int64_t> acc = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		acc = invoke(nativeAdd, acc, val<int64_t>(1));
	}
	return acc;
}
val<int32_t> sumArr(val<int32_t*> array, val<int32_t> length) {
	val<int32_t> s = val<int32_t>(0);
	for (val<int32_t> i = 0; i < length; i = i + 1) {
		val<int32_t> v = array[i];
		s = s + v;
	}
	return s;
}
// branchy: forty ifs in a row, unrolled by a static loop
val<int32_t> chain(val<int32_t> x) {
	val<int32_t> r = 0;
	for (static_val<int> i = 0; i < 40; i++) {
		if (x > i * 3) {
			r = r + (x ^ i);
		} else {
			r = r - i;
		}
	}
	return r;
}
// nested counted loops over a matrix
val<int64_t> nested(val<int32_t*> m, val<int32_t> rows, val<int32_t> cols) {
	val<int64_t> s = 0;
	for (val<int32_t> i = 0; i < rows; i = i + 1) {
		for (val<int32_t> j = 0; j < cols; j = j + 1) {
			val<int32_t> v = m[i * cols + j];
			s = s + static_cast<val<int64_t>>(v);
		}
	}
	return s;
}
// three loads at a stride of 3 elements
val<int32_t> stride3(val<int32_t*> a, val<int32_t> n) {
	val<int32_t> s = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		s = s + a[i * 3] + a[i * 3 + 1] * 2 + a[i * 3 + 2];
	}
	return s;
}
// load, multiply-add, store: y[i] = a * x[i] + y[i]
val<int64_t> axpyI64(val<int64_t*> x, val<int64_t*> y, val<int32_t> n) {
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		y[i] = x[i] * 3 + y[i];
	}
	return y[0];
}
val<double> axpyF64(val<double*> x, val<double*> y, val<int32_t> n) {
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		y[i] = x[i] * 1.5 + y[i];
	}
	return y[0];
}
val<float> dotF32(val<float*> x, val<float*> y, val<int32_t> n) {
	val<float> s = 0.0f;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		s = s + x[i] * y[i];
	}
	return s;
}
// a select in the loop body
val<int64_t> selectLoop(val<int64_t*> x, val<int32_t> n, val<int64_t> t) {
	val<int64_t> acc = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		val<int64_t> v = x[i];
		acc = acc + select(v > t, v, t);
	}
	return acc;
}
// a branch in the loop body, a compare against zero
val<int32_t> countNonZero(val<int32_t*> x, val<int32_t> n) {
	val<int32_t> c = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		if (x[i] != 0) {
			c = c + 1;
		}
	}
	return c;
}
// narrow integers
val<int8_t> narrow8(val<int8_t*> x, val<int32_t> n) {
	val<int8_t> a = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		val<int8_t> v = x[i];
		a = a * val<int8_t>((int8_t) 3) + v;
	}
	return a;
}
val<int16_t> narrow16(val<int16_t*> x, val<int32_t> n) {
	val<int16_t> a = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		val<int16_t> v = x[i];
		a = a * val<int16_t>((int16_t) 5) + v;
	}
	return a;
}
// an internal call with six arguments
val<int64_t> manyArgsBody(val<int64_t> a, val<int64_t> b, val<int64_t> c, val<int64_t> d, val<int64_t> e,
                          val<int64_t> f) {
	return a + b * 2 + c * 3 + d + e + f;
}
static auto manyArgsCallee = NautilusFunction {"manyArgsCallee", manyArgsBody};
val<int64_t> manyArgsLoop(val<int32_t> n) {
	val<int64_t> acc = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		acc = manyArgsCallee(acc, val<int64_t>(i), acc, val<int64_t>(2), val<int64_t>(3), val<int64_t>(4));
	}
	return acc;
}
// the same with a native callee
int64_t nativeMany(int64_t a, int64_t b, int64_t c, int64_t d, int64_t e, int64_t f) noexcept {
	return a + b * 2 + c * 3 + d + e + f;
}
val<int64_t> manyArgsNative(val<int32_t> n) {
	val<int64_t> acc = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		acc = invoke(nativeMany, acc, val<int64_t>(i), acc, val<int64_t>(2), val<int64_t>(3), val<int64_t>(4));
	}
	return acc;
}

static double us(Clock::duration d) {
	return std::chrono::duration<double, std::micro>(d).count();
}

namespace {
constexpr int N = 1 << 16;
std::vector<int32_t> i32Data(1 << 20, 3);
std::vector<int64_t> i64X(N, 2), i64Y(N, 5);
std::vector<double> f64X(N, 1.25), f64Y(N, 0.5);
std::vector<float> f32X(N, 1.5f), f32Y(N, 0.75f);
std::vector<int8_t> i8Data(N, 3);
std::vector<int16_t> i16Data(N, 7);
std::vector<int32_t> zeroish(N);
} // namespace

// A kernel: how to compile it on an engine and how to time one call of it.
struct Entry {
	const char* name;
	std::function<std::function<double(int)>(engine::NautilusEngine&)> prepare; // returns: time(batch) in us per call
};

template <typename F, typename Call>
static Entry entry(const char* name, F f, Call call) {
	return Entry {name, [f, call](engine::NautilusEngine& eng) {
		              auto fn = std::make_shared<decltype(eng.registerFunction(f))>(eng.registerFunction(f));
		              return std::function<double(int)>([fn, call](int batch) {
			              auto s = Clock::now();
			              for (int i = 0; i < batch; i++) {
				              volatile auto y = call(*fn);
				              (void) y;
			              }
			              return us(Clock::now() - s) / batch;
		              });
	              }};
}

static std::vector<Entry> entries() {
	std::vector<Entry> e;
	e.push_back(entry("fib", fib, [](auto& fn) { return fn(15000); }));
	e.push_back(entry("internalCallLoop", internalCallLoop, [](auto& fn) { return fn(10000); }));
	e.push_back(entry("externalCallLoop", externalCallLoop, [](auto& fn) { return fn(10000); }));
	e.push_back(entry("sumArr", sumArr, [](auto& fn) { return fn(i32Data.data(), (int32_t) i32Data.size()); }));
	e.push_back(entry("chain", chain, [](auto& fn) { return fn(57); }));
	e.push_back(entry("nested", nested, [](auto& fn) { return fn(i32Data.data(), 1024, 1024); }));
	e.push_back(entry("stride3", stride3, [](auto& fn) { return fn(i32Data.data(), 100000); }));
	e.push_back(entry("axpyI64", axpyI64, [](auto& fn) { return fn(i64X.data(), i64Y.data(), N); }));
	e.push_back(entry("axpyF64", axpyF64, [](auto& fn) { return fn(f64X.data(), f64Y.data(), N); }));
	e.push_back(entry("dotF32", dotF32, [](auto& fn) { return fn(f32X.data(), f32Y.data(), N); }));
	e.push_back(entry("selectLoop", selectLoop, [](auto& fn) { return fn(i64X.data(), N, int64_t {3}); }));
	e.push_back(entry("countNonZero", countNonZero, [](auto& fn) { return fn(zeroish.data(), N); }));
	e.push_back(entry("narrow8", narrow8, [](auto& fn) { return fn(i8Data.data(), N); }));
	e.push_back(entry("narrow16", narrow16, [](auto& fn) { return fn(i16Data.data(), N); }));
	e.push_back(entry("manyArgsLoop", manyArgsLoop, [](auto& fn) { return fn(10000); }));
	e.push_back(entry("manyArgsNative", manyArgsNative, [](auto& fn) { return fn(10000); }));
	return e;
}

static engine::Options options(bool nir, bool dump) {
	engine::Options op;
	op.setOption("engine.backend", std::string("asmjit"));
	op.setOption("tracing.nirTracer", nir);
	if (dump) {
		op.setOption("dump.after_asmjit_assembly", true);
		op.setOption("dump.console", true);
	}
	return op;
}

static int dumpAll(const std::string& only) {
	for (bool nir : {true, false}) {
		for (const auto& e : entries()) {
			if (!only.empty() && only != e.name) {
				continue;
			}
			std::printf("== %s %s\n", nir ? "asmjit-nir" : "asmjit-0.3", e.name);
			std::fflush(stdout);
			engine::NautilusEngine eng(options(nir, true));
			auto timer = e.prepare(eng);
			(void) timer;
			std::fflush(stdout);
		}
	}
	return 0;
}

int main(int argc, char** argv) {
	if (argc > 1 && std::string(argv[1]) == "dump") {
		return dumpAll(argc > 2 ? argv[2] : "");
	}
	const int reps = argc > 2 ? atoi(argv[2]) : 11;
	const std::string only = argc > 3 ? argv[3] : "";
	std::printf("kernel\tnir_us\told_us\tnir/old\n");
	for (const auto& e : entries()) {
		if (!only.empty() && only != e.name) {
			continue;
		}
		engine::NautilusEngine nirEngine(options(true, false)), oldEngine(options(false, false));
		auto nirTimer = e.prepare(nirEngine);
		auto oldTimer = e.prepare(oldEngine);
		if (std::getenv("ONE_CALL") != nullptr) {
			std::printf("%s one call: nir %.3f old %.3f\n", e.name, nirTimer(1), oldTimer(1));
			continue;
		}
		// the batch is sized so that one timing takes about a millisecond
		int batch = 1;
		while (oldTimer(batch) * batch < 1000.0 && batch < 100000) {
			batch *= 2;
		}
		double bestNir = 1e18, bestOld = 1e18;
		for (int r = 0; r < reps; r++) {
			bestNir = std::min(bestNir, nirTimer(batch));
			bestOld = std::min(bestOld, oldTimer(batch));
		}
		std::printf("%s\t%.3f\t%.3f\t%.3f\n", e.name, bestNir, bestOld, bestNir / bestOld);
		std::fflush(stdout);
	}
	return 0;
}
