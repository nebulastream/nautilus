#include "nautilus/common/ExecutableImage.hpp"

#ifdef __linux__
#include <elf.h>
#include <link.h>
#endif

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <utility>

namespace nautilus::common {

#ifdef __linux__
namespace {

constexpr std::size_t align4(std::size_t value) {
	return (value + 3U) & ~std::size_t {3U};
}

bool containsReadableRange(const dl_phdr_info& image, uintptr_t address, std::size_t size) {
	for (ElfW(Half) i = 0; i < image.dlpi_phnum; ++i) {
		const auto& header = image.dlpi_phdr[i];
		if (header.p_type != PT_LOAD || (header.p_flags & PF_R) == 0 ||
		    header.p_vaddr > std::numeric_limits<uintptr_t>::max() - image.dlpi_addr) {
			continue;
		}
		const auto start = static_cast<uintptr_t>(image.dlpi_addr) + header.p_vaddr;
		if (address >= start && address - start <= header.p_memsz && size <= header.p_memsz - (address - start)) {
			return true;
		}
	}
	return false;
}

std::optional<std::string> imageBuildId(const dl_phdr_info& image) {
	static constexpr char digits[] = "0123456789abcdef";
	for (ElfW(Half) i = 0; i < image.dlpi_phnum; ++i) {
		const auto& header = image.dlpi_phdr[i];
		if (header.p_type != PT_NOTE || header.p_memsz < sizeof(ElfW(Nhdr)) ||
		    header.p_vaddr > std::numeric_limits<uintptr_t>::max() - image.dlpi_addr) {
			continue;
		}
		const auto address = static_cast<uintptr_t>(image.dlpi_addr) + header.p_vaddr;
		if (header.p_memsz > std::numeric_limits<uintptr_t>::max() - address ||
		    !containsReadableRange(image, address, header.p_memsz)) {
			continue;
		}
		const auto* begin = reinterpret_cast<const std::byte*>(address);
		const auto* cursor = begin;
		const auto* end = begin + header.p_memsz;
		while (static_cast<std::size_t>(end - cursor) >= sizeof(ElfW(Nhdr))) {
			ElfW(Nhdr) note {};
			std::memcpy(&note, cursor, sizeof(note));
			cursor += sizeof(note);
			const auto nameSize = align4(note.n_namesz);
			const auto valueSize = align4(note.n_descsz);
			if (nameSize > static_cast<std::size_t>(end - cursor) ||
			    valueSize > static_cast<std::size_t>(end - cursor) - nameSize) {
				break;
			}
			const auto* name = reinterpret_cast<const char*>(cursor);
			const auto* value = cursor + nameSize;
			if (note.n_type == NT_GNU_BUILD_ID && note.n_namesz >= 3 && std::memcmp(name, "GNU", 3) == 0 &&
			    note.n_descsz != 0) {
				std::string result;
				result.reserve(note.n_descsz * 2U);
				for (ElfW(Word) byteIndex = 0; byteIndex < note.n_descsz; ++byteIndex) {
					const auto byte = std::to_integer<uint8_t>(value[byteIndex]);
					result.push_back(digits[byte >> 4U]);
					result.push_back(digits[byte & 0x0fU]);
				}
				return result;
			}
			cursor += nameSize + valueSize;
		}
	}
	return std::nullopt;
}

bool containsExecutableAddress(const dl_phdr_info& image, uintptr_t address) {
	for (ElfW(Half) i = 0; i < image.dlpi_phnum; ++i) {
		const auto& header = image.dlpi_phdr[i];
		if (header.p_type != PT_LOAD || (header.p_flags & PF_X) == 0 ||
		    header.p_vaddr > std::numeric_limits<uintptr_t>::max() - image.dlpi_addr) {
			continue;
		}
		const auto start = static_cast<uintptr_t>(image.dlpi_addr) + header.p_vaddr;
		if (header.p_memsz <= std::numeric_limits<uintptr_t>::max() - start && address >= start &&
		    address < start + header.p_memsz) {
			return true;
		}
	}
	return false;
}

struct LocateContext {
	uintptr_t address;
	std::optional<ExecutableImageLocation> result;
};

int locateCallback(dl_phdr_info* image, std::size_t, void* opaque) {
	auto& context = *static_cast<LocateContext*>(opaque);
	if (!containsExecutableAddress(*image, context.address)) {
		return 0;
	}
	auto buildId = imageBuildId(*image);
	if (!buildId || context.address < static_cast<uintptr_t>(image->dlpi_addr)) {
		return 0;
	}
	context.result =
	    ExecutableImageLocation {std::move(*buildId), context.address - static_cast<uintptr_t>(image->dlpi_addr)};
	return 1;
}

struct ResolveContext {
	const ExecutableImageLocation& location;
	void* result = nullptr;
	std::size_t matches = 0;
};

int resolveCallback(dl_phdr_info* image, std::size_t, void* opaque) {
	auto& context = *static_cast<ResolveContext*>(opaque);
	auto buildId = imageBuildId(*image);
	if (!buildId || *buildId != context.location.buildId ||
	    context.location.loadOffset > std::numeric_limits<uintptr_t>::max() - image->dlpi_addr) {
		return 0;
	}
	const auto address = static_cast<uintptr_t>(image->dlpi_addr) + context.location.loadOffset;
	if (!containsExecutableAddress(*image, address)) {
		return 0;
	}
	context.result = reinterpret_cast<void*>(address);
	context.matches++;
	return 0;
}

} // namespace
#endif

std::optional<ExecutableImageLocation> locateExecutableAddress(const void* address) {
#ifdef __linux__
	if (address == nullptr) {
		return std::nullopt;
	}
	LocateContext context {reinterpret_cast<uintptr_t>(address), std::nullopt};
	dl_iterate_phdr(locateCallback, &context);
	return context.result;
#else
	(void) address;
	return std::nullopt;
#endif
}

void* resolveExecutableAddress(const ExecutableImageLocation& location) {
#ifdef __linux__
	ResolveContext context {location};
	dl_iterate_phdr(resolveCallback, &context);
	return context.matches == 1 ? context.result : nullptr;
#else
	(void) location;
	return nullptr;
#endif
}

std::string stableExecutableAddressName(const void* address) {
	auto location = locateExecutableAddress(address);
	if (!location) {
		return {};
	}
	static constexpr char digits[] = "0123456789abcdef";
	std::string offset;
	auto value = location->loadOffset;
	do {
		offset.push_back(digits[value & 0x0fU]);
		value >>= 4U;
	} while (value != 0);
	std::reverse(offset.begin(), offset.end());
	return "__nautilus_elf_" + location->buildId + "_" + offset;
}

} // namespace nautilus::common
