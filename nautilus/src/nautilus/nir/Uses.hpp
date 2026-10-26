#pragma once

#include "nautilus/nir/Module.hpp"
#include "nautilus/nir/Operands.hpp"
#include <span>
#include <vector>

namespace nautilus::nir {

/// The users of every value of one function in compressed-sparse-row form, built on request (02-nir.md §3.6). It is
/// derived data and never part of an image. Users are function-local instruction ids in ascending order; an
/// instruction that uses a value twice is listed twice.
class UseTable {
public:
	UseTable(const Module& m, uint32_t function);

	std::span<const uint32_t> users(uint32_t valueId) const {
		return std::span<const uint32_t>(users_).subspan(offsets_[valueId], offsets_[valueId + 1] - offsets_[valueId]);
	}
	uint32_t useCount(uint32_t valueId) const {
		return offsets_[valueId + 1] - offsets_[valueId];
	}

private:
	std::vector<uint32_t> offsets_;
	std::vector<uint32_t> users_;
};

} // namespace nautilus::nir
