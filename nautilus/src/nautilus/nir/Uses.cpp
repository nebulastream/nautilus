#include "nautilus/nir/Uses.hpp"

namespace nautilus::nir {

UseTable::UseTable(const Module& m, uint32_t function) {
	auto& fn = m.functions[function];
	auto insts = m.functionInsts(function);
	auto level = static_cast<Level>(fn.level);
	offsets_.assign(insts.size() + 1, 0);
	for (auto& inst : insts) {
		forEachRef(inst, m.extra.data(), level, [&](Ref r) {
			if (isValueRef(r)) {
				offsets_[r + 1]++;
			}
		});
	}
	for (size_t i = 1; i < offsets_.size(); ++i) {
		offsets_[i] += offsets_[i - 1];
	}
	users_.resize(offsets_.back());
	std::vector<uint32_t> fill(offsets_.begin(), offsets_.end() - 1);
	for (uint32_t i = 0; i < insts.size(); ++i) {
		forEachRef(insts[i], m.extra.data(), level, [&](Ref r) {
			if (isValueRef(r)) {
				users_[fill[r]++] = i;
			}
		});
	}
}

} // namespace nautilus::nir
