#include "nautilus/compiler/backends/PerfJitDump.hpp"

#if defined(__linux__)

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <filesystem>
#include <mutex>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

namespace nautilus::compiler {

namespace {

// jitdump-specification.txt: a 40-byte header, then records of a 16-byte prefix (id, total size, timestamp) and a body.
constexpr uint32_t MAGIC = (uint32_t('J') << 24) | (uint32_t('i') << 16) | (uint32_t('T') << 8) | uint32_t('D');
constexpr uint32_t VERSION = 1;
constexpr uint32_t RECORD_CODE_LOAD = 0;
constexpr uint32_t RECORD_DEBUG_INFO = 2;
constexpr uint32_t RECORD_CLOSE = 3;

#if defined(__aarch64__)
constexpr uint32_t ELF_MACHINE = 183; // EM_AARCH64
#else
constexpr uint32_t ELF_MACHINE = 62; // EM_X86_64
#endif

// perf record -k mono stamps events with CLOCK_MONOTONIC, and perf inject matches records to samples by time
uint64_t timestamp() {
	timespec now {};
	clock_gettime(CLOCK_MONOTONIC, &now);
	return static_cast<uint64_t>(now.tv_sec) * 1000000000ull + static_cast<uint64_t>(now.tv_nsec);
}

template <class T>
void put(std::string& out, T value) {
	out.append(reinterpret_cast<const char*>(&value), sizeof(value));
}

void putPrefix(std::string& out, uint32_t id, uint32_t totalSize) {
	put<uint32_t>(out, id);
	put<uint32_t>(out, totalSize);
	put<uint64_t>(out, timestamp());
}

struct Dump {
	std::mutex mutex;
	int fd = -1;
	void* marker = nullptr;
	uint64_t codeIndex = 0;
	bool failed = false;

	~Dump() {
		if (fd < 0) {
			return;
		}
		// the record `perf inject` takes as "the dump is complete"
		std::string record;
		putPrefix(record, RECORD_CLOSE, 16);
		(void) !::write(fd, record.data(), record.size());
		if (marker != nullptr) {
			munmap(marker, static_cast<size_t>(sysconf(_SC_PAGESIZE)));
		}
		::close(fd);
	}

	/// Opens the file and maps its first page executable: perf finds the file through that mapping's event.
	bool open() {
		if (fd >= 0) {
			return true;
		}
		if (failed) {
			return false;
		}
		failed = true;
		const char* base = std::getenv("JITDUMPDIR");
		if (base == nullptr || *base == '\0') {
			base = std::getenv("HOME");
		}
		if (base == nullptr || *base == '\0') {
			return false;
		}
		std::error_code ec;
		const auto dir = std::filesystem::path(base) / ".debug" / "jit" /
		                 ("nautilus-" + std::to_string(::getpid()) + "-" + std::to_string(timestamp()));
		std::filesystem::create_directories(dir, ec);
		if (ec) {
			return false;
		}
		const auto path = (dir / ("jit-" + std::to_string(::getpid()) + ".dump")).string();
		fd = ::open(path.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0666);
		if (fd < 0) {
			return false;
		}
		std::string header;
		put<uint32_t>(header, MAGIC);
		put<uint32_t>(header, VERSION);
		put<uint32_t>(header, 40);
		put<uint32_t>(header, ELF_MACHINE);
		put<uint32_t>(header, 0);
		put<uint32_t>(header, static_cast<uint32_t>(::getpid()));
		put<uint64_t>(header, timestamp());
		put<uint64_t>(header, 0);
		if (::write(fd, header.data(), header.size()) != static_cast<ssize_t>(header.size())) {
			::close(fd);
			fd = -1;
			return false;
		}
		marker = mmap(nullptr, static_cast<size_t>(sysconf(_SC_PAGESIZE)), PROT_READ | PROT_EXEC, MAP_PRIVATE, fd, 0);
		if (marker == MAP_FAILED) {
			marker = nullptr;
		}
		failed = false;
		return true;
	}
};

Dump& dump() {
	static Dump instance;
	return instance;
}

} // namespace

PerfJitDump& PerfJitDump::instance() {
	static PerfJitDump writer;
	return writer;
}

void PerfJitDump::writeCode(const std::string& name, uintptr_t address, size_t size,
                            const std::vector<JitDumpLine>& lines) {
	if (size == 0) {
		return;
	}
	auto& d = dump();
	const std::lock_guard lock(d.mutex);
	if (!d.open()) {
		return;
	}
	std::string out;
	if (!lines.empty()) {
		uint64_t total = 16 + 16;
		for (const auto& line : lines) {
			total += 16 + line.file.size() + 1;
		}
		putPrefix(out, RECORD_DEBUG_INFO, static_cast<uint32_t>(total));
		put<uint64_t>(out, address);
		put<uint64_t>(out, lines.size());
		for (const auto& line : lines) {
			put<uint64_t>(out, line.address);
			put<uint32_t>(out, line.line);
			put<uint32_t>(out, 0); // discriminator
			out.append(line.file);
			out.push_back('\0');
		}
	}
	// pid, tid, vma, code address, code size, code index, the NUL-terminated name, then the code itself
	putPrefix(out, RECORD_CODE_LOAD, static_cast<uint32_t>(16 + 40 + name.size() + 1 + size));
	put<uint32_t>(out, static_cast<uint32_t>(::getpid()));
	put<uint32_t>(out, static_cast<uint32_t>(syscall(SYS_gettid)));
	put<uint64_t>(out, address);
	put<uint64_t>(out, address);
	put<uint64_t>(out, size);
	put<uint64_t>(out, d.codeIndex++);
	out.append(name);
	out.push_back('\0');
	out.append(reinterpret_cast<const char*>(address), size);
	(void) !::write(d.fd, out.data(), out.size());
}

} // namespace nautilus::compiler

#else

namespace nautilus::compiler {

PerfJitDump& PerfJitDump::instance() {
	static PerfJitDump writer;
	return writer;
}

void PerfJitDump::writeCode(const std::string&, uintptr_t, size_t, const std::vector<JitDumpLine>&) {
}

} // namespace nautilus::compiler

#endif
