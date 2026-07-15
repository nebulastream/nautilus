// Smoke fixture: nautilus-std plugin — proves plugin headers and the
// whole-archive plugin link in libplayground_core.so work end-to-end.
#include <nautilus/Engine.hpp>
#include <nautilus/std/cmath.h>
#include <nautilus/std/vector.h>
#include <nautilus/val.hpp>
#include <vector>

using namespace nautilus;

void playground_register(engine::NautilusModule& m) {
	// Root-mean-square over a std::vector<double>, using nautilus::sqrt from
	// the cmath wrapper. The caller owns the vector, so the val wrapper is
	// released before returning.
	m.registerFunction<val<double>(val<std::vector<double>*>)>("rootMeanSquare", [](val<std::vector<double>*> v_ptr) {
		val<std::vector<double>> v(v_ptr);
		val<double> sum = 0.0;
		val<double> count = 0.0;
		for (val<size_t> i = 0; i < v.size(); i++) {
			sum += v[i] * v[i];
			count += 1.0;
		}
		val<double> rms = nautilus::sqrt(sum / count);
		v.release();
		return rms;
	});
}
