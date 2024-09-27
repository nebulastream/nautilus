#pragma once

#include <nautilus/Engine.hpp>

namespace nautilus::engine {

val<int32_t> ifInsideLoop(val<int32_t> value) {
	val<int32_t> result = 0;
	for (val<int32_t> i = 0; i < value; i = i + 1) {
		if (i % 2 == 0) {
			result += 1;
		}
	}
	return result;
}

val<int32_t> sumLoop(val<int32_t> upperLimit) {
	val<int32_t> agg = val<int32_t>(1);
	for (val<int32_t> start = 0; start < upperLimit; start = start + 1) {
		agg = agg + 10;
	}
	return agg;
}

val<int32_t> nestedSumLoop(val<int32_t> upperLimit) {
	val<int32_t> agg = val<int32_t>(1);
	for (val<int32_t> start = 0; start < upperLimit; start = start + 1) {
		for (val<int32_t> start2 = 0; start2 < upperLimit; start2 = start2 + 1) {
			agg = agg + 10;
		}
	}
	return agg;
}

val<int32_t> ifSumLoop() {
	val<int32_t> agg = val<int32_t>(1);
	for (val<int32_t> start = 0; start < 10; start = start + 1) {
		if (agg < 50) {
			agg = agg + 10;
		}
	}
	return agg;
}

val<int32_t> ifElseSumLoop() {
	val<int32_t> agg = val<int32_t>(1);
	for (val<int32_t> start = 0; start < 10; start = start + 1) {
		if (agg < 50) {
			agg = agg + 10;
		} else {
			agg = agg + 1;
		}
	}
	return agg;
}

val<int32_t> elseOnlySumLoop() {
	val<int32_t> agg = val<int32_t>(1);
	for (val<int32_t> start = 0; start < 10; start = start + 1) {
		if (agg < 50) {
		} else {
			agg = agg + 1;
		}
	}
	return agg;
}

val<int32_t> nestedIfSumLoop() {
	val<int32_t> agg = val<int32_t>(1);
	for (val<int32_t> start = 0; start < 10; start = start + 1) {
		if (agg < 50) {
			if (agg < 40) {
				agg = agg + 10;
			}
		} else {
			agg = agg + 1;
		}
	}
	return agg;
}

val<int32_t> nestedIfElseSumLoop() {
	val<int32_t> agg = val<int32_t>(1);
	for (val<int32_t> start = 0; start < 10; start = start + 1) {
		if (agg < 50) {
			if (agg < 40) {
				agg = agg + 10;
			} else {
				agg = agg + 100;
			}
		} else {
			agg = agg + 1;
		}
	}
	return agg;
}

val<int32_t> nestedElseOnlySumLoop() {
	val<int32_t> agg = val<int32_t>(1);
	for (val<int32_t> start = 0; start < 10; start = start + 1) {
		if (agg < 50) {
			if (agg < 40) {
			} else {
				agg = agg + 100;
			}
		} else {
			agg = agg + 1;
		}
	}
	return agg;
}

val<int32_t> sumOfNumbers(val<int32_t> n) {
	val<int32_t> sum = 0;
	for (val<int> i = 1; i <= n; i = i + 1) {
		sum += i;
	}
	return sum;
}

val<int32_t> factorial(val<int32_t> n) {
	val<int32_t> result = 1;
	while (n > 1) {
		result *= n;
		n = n - 1;
	}
	return result;
}

val<int32_t> reverseNumber(val<int32_t> n) {
	val<int32_t> reversed = 0;
	do {
		auto mul = reversed * 10;
		auto mod = n % 10;
		reversed = mul + mod;
		n /= 10;
	} while (n > 0);
	return reversed;
}

val<int32_t> fibonacci(val<int32_t> n) {
	val<int32_t> a = 0, b = 1, c;
	for (val<int> i = 2; i <= n; i = i + 1) {
		c = a + b;
		a = b;
		b = c;
	}
	return b;
}

val<int32_t> gcd(val<int32_t> a, val<int32_t> b) {
	while (b != 0) {
		val<int32_t> t = b;
		b = a % b;
		a = t;
	}
	return a;
}

val<int32_t> decimalToBinary(val<int32_t> n) {
	val<int32_t> binary = 0, base = 1;
	do {
		binary += (n % 2) * base;
		base *= 10;
		n /= 2;
	} while (n > 0);
	return binary;
}

val<bool> isPrime(val<int32_t> n) {
	auto f = val<bool>(false);
	auto t = val<bool>(true);
	if (n <= 1)
		return f;
	for (val<int32_t> i = 2; i * i <= n; i = i + 1) {
		if (n % i == 0)
			return f;
	}
	return t;
}

val<int32_t> collatz(val<int32_t> n) {
	val<int32_t> steps = 0;
	while (n != 1) {
		if (n % 2 == 0) {
			n = n / 2;
		} else {
			n = 3 * n + 1;
		}
		steps = steps + 1;
	}
	return steps;
}

val<int32_t> digitSum(val<int32_t> n) {
	val<int32_t> sum = 0;
	do {
		sum += n % 10;
		n /= 10;
	} while (n > 0);
	return sum;
}

val<int32_t> sumOfSquares(val<int32_t> n) {
	val<int32_t> sum = 0;
	for (val<int> i = 1; i <= n; i = i + 1) {
		sum += i * i;
	}
	return sum;
}

val<int32_t> countDigits(val<int32_t> n) {
	val<int32_t> count = 0;
	while (n != 0) {
		n /= 10;
		count = count + 1;
	}
	return count;
}

} // namespace nautilus::engine
