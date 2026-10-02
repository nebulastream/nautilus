#pragma once

#include "NestedIfBenchmarks.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/region.hpp"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <functional>
#include <memory>
#include <random>
#include <string>
#include <vector>

/// The benchmark suite the backend explorer (nautilus-backend-explorer, see tools/backend-explorer) compiles and runs
/// under every backend configuration. Each kernel owns its inputs, so a compiled kernel can be run any number of
/// times, and returns an int64 checksum, so a configuration that miscompiles -- e.g. an ablated LLVM pipeline -- is
/// caught by comparing checksums across configurations rather than silently reported as fast.
///
/// Kernels are sized so one run takes well under a millisecond on the MLIR backend at -O3 and tens of milliseconds on
/// the interpreters, which keeps a full sweep tractable.
namespace nautilus::engine::benchmark {

/// A kernel compiled under one configuration: `run` executes it once over its inputs and returns the checksum.
struct CompiledKernel {
	std::function<int64_t()> run;
	std::shared_ptr<const compiler::CompilationStatistics> statistics;
};

struct BenchmarkKernel {
	std::string name;
	/// micro | query | control-flow | calls
	std::string category;
	std::string description;
	/// Traces and compiles the kernel on @p engine. Input data is prepared before, never inside, this call.
	std::function<CompiledKernel(NautilusEngine&)> compile;
};

namespace detail {

template <typename Fn, typename Invoke>
CompiledKernel compileKernel(NautilusEngine& engine, Fn fn, Invoke invoke) {
	using Compiled = decltype(engine.registerFunction(fn));
	auto compiled = std::make_shared<Compiled>(engine.registerFunction(fn));
	auto statistics = compiled->getStatistics();
	return {[compiled, invoke] { return static_cast<int64_t>(invoke(*compiled)); }, std::move(statistics)};
}

inline std::vector<int32_t> randomInts(size_t n, int32_t lo, int32_t hi, uint32_t seed) {
	std::mt19937 rng(seed);
	std::uniform_int_distribution<int32_t> dist(lo, hi);
	std::vector<int32_t> data(n);
	for (auto& v : data) {
		v = dist(rng);
	}
	return data;
}

} // namespace detail

// ---------------------------------------------------------------------------------------------------------------
// micro: arithmetic and memory loops

inline val<int8_t> benchAdd(val<int8_t> x) {
	val<int8_t> y = (int8_t) 2;
	return y + x;
}

/// Unsigned so the wrap-around is defined: signed overflow would let an optimizing configuration change the result.
inline val<uint32_t> benchFibonacci(val<int32_t> n) {
	val<uint32_t> a = (uint32_t) 0, b = (uint32_t) 1, c;
	for (val<int32_t> i = 2; i <= n; i = i + 1) {
		c = a + b;
		a = b;
		b = c;
	}
	return b;
}

inline val<int64_t> benchArraySum(val<int32_t*> array, val<int32_t> length) {
	val<int64_t> sum = 0;
	for (val<int32_t> i = 0; i < length; i = i + 1) {
		val<int32_t> value = array[i];
		val<int64_t> wide = value;
		sum = sum + wide;
	}
	return sum;
}

inline val<int64_t> benchPrefixSum(val<int32_t*> in, val<int64_t*> out, val<int32_t> length) {
	val<int64_t> running = 0;
	for (val<int32_t> i = 0; i < length; i = i + 1) {
		val<int32_t> value = in[i];
		val<int64_t> wide = value;
		running = running + wide;
		out[i] = running;
	}
	return out[length - 1];
}

/// Murmur-style mixing over every element: shifts, xors and 64-bit multiplies, no branches.
inline val<uint64_t> benchHashMix(val<uint64_t*> data, val<int32_t> length) {
	val<uint64_t> h = (uint64_t) 0x9e3779b97f4a7c15ULL;
	for (val<int32_t> i = 0; i < length; i = i + 1) {
		val<uint64_t> k = data[i];
		k = k * (uint64_t) 0x87c37b91114253d5ULL;
		k = (k << (uint64_t) 31) | (k >> (uint64_t) 33);
		k = k * (uint64_t) 0x4cf5ad432745937fULL;
		h = h ^ k;
		h = (h << (uint64_t) 27) | (h >> (uint64_t) 37);
		h = h * (uint64_t) 5 + (uint64_t) 0x52dce729;
	}
	h = h ^ (h >> (uint64_t) 33);
	return h;
}

inline val<double> benchMatMul(val<double*> a, val<double*> b, val<double*> c, val<int32_t> n) {
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		for (val<int32_t> j = 0; j < n; j = j + 1) {
			val<double> acc = 0.0;
			for (val<int32_t> k = 0; k < n; k = k + 1) {
				val<double> x = a[i * n + k];
				val<double> y = b[k * n + j];
				acc = acc + x * y;
			}
			c[i * n + j] = acc;
		}
	}
	val<double> trace = 0.0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		val<double> v = c[i * n + i];
		trace = trace + v;
	}
	return trace;
}

// ---------------------------------------------------------------------------------------------------------------
// query: shapes query compilation produces (scan, filter, aggregate, probe)

inline val<int64_t> benchFilterSum(val<int32_t*> array, val<int32_t> length) {
	val<int64_t> sum = 0;
	for (val<int32_t> i = 0; i < length; i = i + 1) {
		val<int32_t> value = array[i];
		if (value % 3 == 0) {
			sum = sum + value;
		}
	}
	return sum;
}

struct LineItem {
	int64_t quantity;
	int64_t extendedPrice;
	int64_t discount;
	int32_t shipDate;
	int32_t padding;
};

/// TPC-H Q6: sum(price * discount) over a row-layout lineitem with a conjunctive range predicate.
inline val<int64_t> benchQ6(val<LineItem*> rows, val<int32_t> count) {
	val<int64_t> revenue = 0;
	for (val<int32_t> i = 0; i < count; i = i + 1) {
		val<LineItem*> row = rows + i;
		val<int32_t> shipDate = row.get(&LineItem::shipDate);
		val<int64_t> discount = row.get(&LineItem::discount);
		val<int64_t> quantity = row.get(&LineItem::quantity);
		if (shipDate >= 8766 && shipDate < 9131 && discount >= 5 && discount <= 7 && quantity < 24) {
			val<int64_t> price = row.get(&LineItem::extendedPrice);
			revenue = revenue + price * discount;
		}
	}
	return revenue;
}

/// Dense group-by: agg[key] += value, keys in [0, groups).
inline val<int64_t> benchGroupBy(val<int32_t*> keys, val<int32_t*> values, val<int64_t*> agg, val<int32_t> count,
                                 val<int32_t> groups) {
	for (val<int32_t> g = 0; g < groups; g = g + 1) {
		agg[g] = (int64_t) 0;
	}
	for (val<int32_t> i = 0; i < count; i = i + 1) {
		val<int32_t> key = keys[i];
		val<int32_t> value = values[i];
		val<int64_t> wide = value;
		val<int64_t> current = agg[key];
		agg[key] = current + wide;
	}
	val<int64_t> checksum = 0;
	for (val<int32_t> g = 0; g < groups; g = g + 1) {
		val<int64_t> v = agg[g];
		val<int64_t> weight = g + 1;
		checksum = checksum + v * weight;
	}
	return checksum;
}

/// Hash-join probe into an open-addressing table (linear probing, key 0 = empty) built natively.
inline val<int64_t> benchHashProbe(val<int64_t*> tableKeys, val<int64_t*> tablePayloads, val<uint64_t> mask,
                                   val<int64_t*> probes, val<int32_t> count) {
	val<int64_t> sum = 0;
	for (val<int32_t> i = 0; i < count; i = i + 1) {
		val<int64_t> key = probes[i];
		val<uint64_t> slot = (static_cast<val<uint64_t>>(key) * (uint64_t) 0x9e3779b97f4a7c15ULL) >> (uint64_t) 40;
		slot = slot & mask;
		val<int64_t> candidate = tableKeys[slot];
		while (candidate != 0) {
			if (candidate == key) {
				val<int64_t> payload = tablePayloads[slot];
				sum = sum + payload;
			}
			slot = (slot + (uint64_t) 1) & mask;
			candidate = tableKeys[slot];
		}
	}
	return sum;
}

inline val<int64_t> benchBinarySearch(val<int32_t*> sorted, val<int32_t> length, val<int32_t*> queries,
                                      val<int32_t> count) {
	val<int64_t> found = 0;
	for (val<int32_t> q = 0; q < count; q = q + 1) {
		val<int32_t> needle = queries[q];
		val<int32_t> lo = 0;
		val<int32_t> hi = length;
		while (lo < hi) {
			val<int32_t> mid = lo + (hi - lo) / 2;
			val<int32_t> value = sorted[mid];
			if (value < needle) {
				lo = mid + 1;
			} else {
				hi = mid;
			}
		}
		found = found + lo;
	}
	return found;
}

// ---------------------------------------------------------------------------------------------------------------
// control-flow: data-dependent loops and large CFGs

inline val<int64_t> benchGcdSum(val<int32_t> n) {
	val<int64_t> total = 0;
	for (val<int32_t> i = 1; i < n; i = i + 1) {
		val<int32_t> a = i;
		val<int32_t> b = n - i + 7;
		while (b != 0) {
			val<int32_t> t = a % b;
			a = b;
			b = t;
		}
		total = total + a;
	}
	return total;
}

inline val<int64_t> benchCollatz(val<int64_t> limit) {
	val<int64_t> steps = 0;
	for (val<int64_t> start = 1; start < limit; start = start + 1) {
		val<int64_t> x = start;
		while (x != 1) {
			if ((x & 1) == 0) {
				x = x >> 1;
			} else {
				x = x * 3 + 1;
			}
			steps = steps + 1;
		}
	}
	return steps;
}

inline val<int32_t> benchSieve(val<uint8_t*> composite, val<int32_t> n) {
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		composite[i] = (uint8_t) 0;
	}
	val<int32_t> primes = 0;
	val<int64_t> limit = n;
	for (val<int32_t> i = 2; i < n; i = i + 1) {
		val<uint8_t> flag = composite[i];
		if (flag == (uint8_t) 0) {
			primes = primes + 1;
			val<int64_t> step = i;
			for (val<int64_t> j = step * step; j < limit; j = j + step) {
				composite[j] = (uint8_t) 1;
			}
		}
	}
	return primes;
}

inline val<int64_t> benchInsertionSort(val<int32_t*> data, val<int32_t> n) {
	for (val<int32_t> i = 1; i < n; i = i + 1) {
		val<int32_t> key = data[i];
		val<int32_t> j = i - 1;
		// A traced && evaluates both operands, so `data[j]` must not be read once j is -1.
		val<bool> shifting = true;
		while (shifting && j >= 0) {
			val<int32_t> current = data[j];
			if (current > key) {
				data[j + 1] = current;
				j = j - 1;
			} else {
				shifting = false;
			}
		}
		data[j + 1] = key;
	}
	val<int64_t> checksum = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		val<int32_t> v32 = data[i];
		val<int64_t> v = v32;
		checksum = ((checksum * 31) & 0xffffffffffffLL) + v;
	}
	return checksum;
}

// ---------------------------------------------------------------------------------------------------------------
// calls: internal (Nautilus-to-Nautilus) and external (invoke) calls

inline val<int64_t> benchCalleeBody(val<int64_t> x, val<int64_t> y) {
	return x * 2 + y;
}

inline NautilusFunction benchCallee {"benchCallee", benchCalleeBody};

inline val<int64_t> benchInternalCall(val<int32_t> n) {
	val<int64_t> acc = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		acc = benchCallee(acc, val<int64_t>(1)) & 0xffffff;
	}
	return acc;
}

__attribute__((noinline)) inline int64_t benchNativeAdd(int64_t a, int64_t b) noexcept {
	return a + b;
}

inline val<int64_t> benchExternalCall(val<int32_t> n) {
	val<int64_t> acc = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		acc = invoke(benchNativeAdd, acc, val<int64_t>(1));
	}
	return acc;
}

__attribute__((noinline)) inline int64_t benchNativeMix(int64_t acc, int64_t v) noexcept {
	return static_cast<int64_t>((static_cast<uint64_t>(acc) ^ (static_cast<uint64_t>(v) * 2654435761ULL)) +
	                            0x9e3779b97f4a7c15ULL);
}

inline val<int64_t> benchCompositeStepBody(val<int64_t> acc, val<int64_t> v) {
	return acc + v * 3 - (v >> 1);
}

inline NautilusFunction benchCompositeStep {"benchCompositeStep", benchCompositeStepBody};

/// The shape of PerfFixtureFunctions.hpp's perfCompositeKernel: nested regions, a data-dependent branch, an internal
/// and an external call. The element is loaded into a val before invoke(): passing `data[i]` (a val<int64_t&>)
/// straight to invoke() hands the callee an address instead of the element, which makes the result depend on where
/// the input lives.
inline val<int64_t> benchComposite(val<int64_t*> data, val<int32_t> len, val<int32_t> rounds) {
	val<int64_t> acc = 0;
	region("outer", [&]() {
		region("hot", [&]() {
			for (val<int32_t> r = 0; r < rounds; r = r + 1) {
				for (val<int32_t> i = 0; i < len; i = i + 1) {
					val<int64_t> v = data[i];
					if (v > 0) {
						acc = benchCompositeStep(acc, v);
					} else {
						acc = acc - v;
					}
				}
			}
		});
		region("mix", [&]() {
			for (val<int32_t> i = 0; i < len; i = i + 1) {
				val<int64_t> v = data[i];
				acc = invoke(benchNativeMix, acc, v);
			}
		});
	});
	return acc;
}

// ---------------------------------------------------------------------------------------------------------------

/// The suite. Inputs are created here, once per process, and captured by the kernels' run closures.
inline std::vector<BenchmarkKernel> benchmarkKernels() {
	using detail::compileKernel;
	using detail::randomInts;
	std::vector<BenchmarkKernel> kernels;

	kernels.push_back({"add", "micro", "A single int8 addition: the fixed per-compilation overhead.",
	                   [](NautilusEngine& e) { return compileKernel(e, benchAdd, [](auto& f) { return f(42); }); }});

	kernels.push_back(
	    {"fibonacci", "micro", "Iterative Fibonacci (loop-carried dependency, 200k iterations).",
	     [](NautilusEngine& e) { return compileKernel(e, benchFibonacci, [](auto& f) { return f(200000); }); }});

	{
		auto data = std::make_shared<std::vector<int32_t>>(randomInts(1 << 18, -1000, 1000, 1));
		kernels.push_back(
		    {"arraySum", "micro", "Sum of 256k int32 values (vectorizable reduction).", [data](NautilusEngine& e) {
			     return compileKernel(e, benchArraySum,
			                          [data](auto& f) { return f(data->data(), static_cast<int32_t>(data->size())); });
		     }});
	}

	{
		auto in = std::make_shared<std::vector<int32_t>>(randomInts(1 << 18, -1000, 1000, 2));
		auto out = std::make_shared<std::vector<int64_t>>(in->size());
		kernels.push_back({"prefixSum", "micro", "Running sum of 256k values written to an output array.",
		                   [in, out](NautilusEngine& e) {
			                   return compileKernel(e, benchPrefixSum, [in, out](auto& f) {
				                   return f(in->data(), out->data(), static_cast<int32_t>(in->size()));
			                   });
		                   }});
	}

	{
		auto data = std::make_shared<std::vector<uint64_t>>(1 << 17);
		std::mt19937_64 rng(3);
		for (auto& v : *data) {
			v = rng();
		}
		kernels.push_back({"hashMix", "micro", "Murmur-style mixing of 128k uint64 values (shifts, xor, mul).",
		                   [data](NautilusEngine& e) {
			                   return compileKernel(e, benchHashMix, [data](auto& f) {
				                   return static_cast<int64_t>(f(data->data(), static_cast<int32_t>(data->size())));
			                   });
		                   }});
	}

	{
		constexpr int32_t n = 64;
		auto a = std::make_shared<std::vector<double>>(n * n);
		auto b = std::make_shared<std::vector<double>>(n * n);
		auto c = std::make_shared<std::vector<double>>(n * n);
		for (int32_t i = 0; i < n * n; ++i) {
			(*a)[i] = (i % 17) * 0.25;
			(*b)[i] = (i % 13) * 0.5;
		}
		kernels.push_back(
		    {"matMul", "micro", "64x64 double matrix multiply (triple loop nest).", [a, b, c](NautilusEngine& e) {
			     return compileKernel(e, benchMatMul, [a, b, c](auto& f) {
				     return static_cast<int64_t>(f(a->data(), b->data(), c->data(), n));
			     });
		     }});
	}

	{
		auto data = std::make_shared<std::vector<int32_t>>(randomInts(1 << 18, 0, 1 << 20, 4));
		kernels.push_back({"filterSum", "query", "Selection (value % 3 == 0) + sum over 256k int32 values.",
		                   [data](NautilusEngine& e) {
			                   return compileKernel(e, benchFilterSum, [data](auto& f) {
				                   return f(data->data(), static_cast<int32_t>(data->size()));
			                   });
		                   }});
	}

	{
		auto rows = std::make_shared<std::vector<LineItem>>(1 << 16);
		std::mt19937 rng(5);
		for (auto& row : *rows) {
			row.quantity = 1 + static_cast<int64_t>(rng() % 50);
			row.extendedPrice = 100 + static_cast<int64_t>(rng() % 100000);
			row.discount = static_cast<int64_t>(rng() % 11);
			row.shipDate = 8000 + static_cast<int32_t>(rng() % 2500);
			row.padding = 0;
		}
		kernels.push_back({"tpchQ6", "query", "TPC-H Q6 over 64k row-layout lineitems (conjunctive filter + sum).",
		                   [rows](NautilusEngine& e) {
			                   return compileKernel(e, benchQ6, [rows](auto& f) {
				                   return f(rows->data(), static_cast<int32_t>(rows->size()));
			                   });
		                   }});
	}

	{
		constexpr int32_t groups = 1024;
		auto keys = std::make_shared<std::vector<int32_t>>(randomInts(1 << 17, 0, groups - 1, 6));
		auto values = std::make_shared<std::vector<int32_t>>(randomInts(1 << 17, 0, 1000, 7));
		auto agg = std::make_shared<std::vector<int64_t>>(groups);
		kernels.push_back({"groupBy", "query", "Dense group-by sum of 128k rows into 1024 groups.",
		                   [keys, values, agg](NautilusEngine& e) {
			                   return compileKernel(e, benchGroupBy, [keys, values, agg](auto& f) {
				                   return f(keys->data(), values->data(), agg->data(),
				                            static_cast<int32_t>(keys->size()), groups);
			                   });
		                   }});
	}

	{
		constexpr uint64_t capacity = 1 << 16;
		auto tableKeys = std::make_shared<std::vector<int64_t>>(capacity, 0);
		auto tablePayloads = std::make_shared<std::vector<int64_t>>(capacity, 0);
		std::mt19937_64 rng(8);
		std::vector<int64_t> buildKeys;
		for (uint64_t i = 0; i < capacity / 2; ++i) {
			int64_t key = static_cast<int64_t>(rng() >> 4) + 1;
			buildKeys.push_back(key);
			uint64_t slot = ((static_cast<uint64_t>(key) * 0x9e3779b97f4a7c15ULL) >> 40) & (capacity - 1);
			while ((*tableKeys)[slot] != 0) {
				slot = (slot + 1) & (capacity - 1);
			}
			(*tableKeys)[slot] = key;
			(*tablePayloads)[slot] = static_cast<int64_t>(i);
		}
		auto probes = std::make_shared<std::vector<int64_t>>(1 << 16);
		for (size_t i = 0; i < probes->size(); ++i) {
			(*probes)[i] = (i % 2 == 0) ? buildKeys[rng() % buildKeys.size()] : static_cast<int64_t>(rng() >> 4) + 1;
		}
		kernels.push_back({"hashProbe", "query",
		                   "Hash-join probe of 64k keys into a 64k-slot linear-probing table (50% hit rate).",
		                   [tableKeys, tablePayloads, probes](NautilusEngine& e) {
			                   return compileKernel(e, benchHashProbe, [tableKeys, tablePayloads, probes](auto& f) {
				                   return f(tableKeys->data(), tablePayloads->data(), capacity - 1, probes->data(),
				                            static_cast<int32_t>(probes->size()));
			                   });
		                   }});
	}

	{
		auto sorted = std::make_shared<std::vector<int32_t>>(randomInts(1 << 16, 0, 1 << 24, 9));
		std::sort(sorted->begin(), sorted->end());
		auto queries = std::make_shared<std::vector<int32_t>>(randomInts(1 << 15, 0, 1 << 24, 10));
		kernels.push_back({"binarySearch", "query", "32k lower-bound lookups in a sorted 64k int32 array.",
		                   [sorted, queries](NautilusEngine& e) {
			                   return compileKernel(e, benchBinarySearch, [sorted, queries](auto& f) {
				                   return f(sorted->data(), static_cast<int32_t>(sorted->size()), queries->data(),
				                            static_cast<int32_t>(queries->size()));
			                   });
		                   }});
	}

	kernels.push_back(
	    {"gcdSum", "control-flow", "Euclid's algorithm for 20k pairs (data-dependent inner loop).",
	     [](NautilusEngine& e) { return compileKernel(e, benchGcdSum, [](auto& f) { return f(20000); }); }});

	kernels.push_back(
	    {"collatz", "control-flow", "Collatz step counts for 1..5000 (unpredictable branches).",
	     [](NautilusEngine& e) { return compileKernel(e, benchCollatz, [](auto& f) { return f(5000); }); }});

	{
		auto buffer = std::make_shared<std::vector<uint8_t>>(1 << 18);
		kernels.push_back({"sieve", "control-flow", "Sieve of Eratosthenes up to 256k.", [buffer](NautilusEngine& e) {
			                   return compileKernel(e, benchSieve, [buffer](auto& f) {
				                   return f(buffer->data(), static_cast<int32_t>(buffer->size()));
			                   });
		                   }});
	}

	{
		auto input = std::make_shared<std::vector<int32_t>>(randomInts(1024, -100000, 100000, 11));
		auto scratch = std::make_shared<std::vector<int32_t>>(input->size());
		kernels.push_back({"insertionSort", "control-flow", "Insertion sort of 1024 int32 values (copied per run).",
		                   [input, scratch](NautilusEngine& e) {
			                   return compileKernel(e, benchInsertionSort, [input, scratch](auto& f) {
				                   std::memcpy(scratch->data(), input->data(), input->size() * sizeof(int32_t));
				                   return f(scratch->data(), static_cast<int32_t>(scratch->size()));
			                   });
		                   }});
	}

	kernels.push_back({"nestedIf100", "control-flow", "100 nested if-else levels: a large CFG, trivial runtime.",
	                   [](NautilusEngine& e) { return compileKernel(e, nestedIf100, [](auto& f) { return f(42); }); }});

	kernels.push_back(
	    {"chainedIf100", "control-flow", "100 chained if-else blocks: a large CFG, trivial runtime.",
	     [](NautilusEngine& e) { return compileKernel(e, chainedIf100, [](auto& f) { return f(42); }); }});

	kernels.push_back({"internalCall", "calls", "20k calls to a traced Nautilus function.", [](NautilusEngine& e) {
		                   return compileKernel(e, benchInternalCall, [](auto& f) { return f(20000); });
	                   }});

	kernels.push_back({"externalCall", "calls", "20k invoke() calls into a native function.", [](NautilusEngine& e) {
		                   return compileKernel(e, benchExternalCall, [](auto& f) { return f(20000); });
	                   }});

	{
		auto data = std::make_shared<std::vector<int64_t>>(256);
		for (size_t i = 0; i < data->size(); ++i) {
			(*data)[i] = static_cast<int64_t>(i) - 128;
		}
		kernels.push_back({"composite", "calls",
		                   "Nested regions, an internal and an external call, data-dependent loop body.",
		                   [data](NautilusEngine& e) {
			                   return compileKernel(e, benchComposite, [data](auto& f) {
				                   return f(data->data(), static_cast<int32_t>(data->size()), 50);
			                   });
		                   }});
	}

	return kernels;
}

} // namespace nautilus::engine::benchmark
