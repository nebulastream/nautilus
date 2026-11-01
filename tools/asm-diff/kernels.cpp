// asm-diff kernels (tools/asm-diff): loops that run long enough for the quality of the generated code to show.
//   nautilus-asm-kernels run [reps]     steady-state microseconds per call (best of reps), per pipeline (NIR and the
//   0.3 fallback) nautilus-asm-kernels dump           the assembly of every kernel on both pipelines, between "==
//   pipeline kernel"
//                                       lines (tools/asm-diff/loop-diff.py counts the instructions of the loops)
// Same source on every branch, so it also runs against `main`.
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include "nautilus/function.hpp"
#include "nautilus/nautilus_function.hpp"
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <string>
#include <vector>
using namespace nautilus;
using Clock = std::chrono::steady_clock;

val<int8_t> addExpression(val<int8_t> x) {
	val<int8_t> y = (int8_t) 2;
	return y + x;
}
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
// branchy: many paths, stresses tracing and compile time
val<int32_t> chain(val<int32_t> x) {
	val<int32_t> r = 0;
	for (int i = 0; i < 40; i++) {
		if (x > i * 3) {
			r = r + (x ^ i);
		} else {
			r = r - i;
		}
	}
	return r;
}

static double us(Clock::duration d) {
	return std::chrono::duration<double, std::micro>(d).count();
}
static double median(std::vector<double> v) {
	std::sort(v.begin(), v.end());
	return v[v.size() / 2];
}
static std::vector<int32_t> data(1 << 20, 3);

template <typename F, typename Call>
static double steady(engine::Options o, F f, Call call, int reps, int batch) {
	engine::NautilusEngine eng(o);
	auto fn = eng.registerFunction(f);
	volatile auto warm = call(fn);
	(void) warm;
	std::vector<double> t;
	for (int r = 0; r < reps; r++) {
		auto s = Clock::now();
		for (int i = 0; i < batch; i++) {
			volatile auto y = call(fn);
			(void) y;
		}
		t.push_back(us(Clock::now() - s) / batch);
	}
	return *std::min_element(t.begin(), t.end());
}

static int dumpAll() {
	for (bool nir : {true, false}) {
		engine::Options op;
		op.setOption("engine.backend", std::string("asmjit"));
		op.setOption("tracing.nirTracer", nir);
		op.setOption("dump.after_asmjit_assembly", true);
		op.setOption("dump.console", true);
		auto compile = [&](const char* name, auto f) {
			std::printf("== %s %s\n", nir ? "asmjit-nir" : "asmjit-0.3", name);
			std::fflush(stdout);
			engine::NautilusEngine eng(op);
			auto fn = eng.registerFunction(f);
			(void) fn;
			std::fflush(stdout);
		};
		compile("fib", fib);
		compile("internalCallLoop", internalCallLoop);
		compile("externalCallLoop", externalCallLoop);
		compile("sumArr", sumArr);
		compile("chain", chain);
	}
	return 0;
}

int main(int argc, char** argv) {
	if (argc > 1 && std::string(argv[1]) == "dump") {
		return dumpAll();
	}
	const int reps = argc > 2 ? atoi(argv[2]) : 11;
	std::vector<std::pair<std::string, engine::Options>> cfgs;
	for (bool nir : {true, false}) {
		engine::Options op;
		op.setOption("engine.backend", std::string("asmjit"));
		op.setOption("tracing.nirTracer", nir);
		cfgs.emplace_back(nir ? "asmjit-nir" : "asmjit-0.3", op);
	}
	std::printf("pipeline\tkernel\tus_per_call\n");
	for (auto& [name, op] : cfgs) {
		auto row = [&](const char* k, double v) {
			std::printf("%s\t%s\t%.3f\n", name.c_str(), k, v);
			std::fflush(stdout);
		};
		row("fib15000", steady(op, fib, [](auto& fn) { return fn(15000); }, reps, 50));
		row("internalCall10000", steady(op, internalCallLoop, [](auto& fn) { return fn(10000); }, reps, 50));
		row("externalCall10000", steady(op, externalCallLoop, [](auto& fn) { return fn(10000); }, reps, 50));
		row("sum1M", steady(op, sumArr, [](auto& fn) { return fn(data.data(), (int32_t) data.size()); }, reps, 20));
	}
}
