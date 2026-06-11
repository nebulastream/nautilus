
#pragma once

#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/Snapshot.hpp"
#include "nautilus/tracing/tag/TagRecorder.hpp"
#include <cstddef>
#include <cstring>
#include <span>
#include <string>
#include <type_traits>
#include <vector>

namespace nautilus::tracing::serialization {

/**
 * @brief Binary serialization of ExecutionTrace state for the fork-based tracer.
 *
 * Both endpoints are forks of the same binary on the same machine, so plain memcpy
 * encoding of trivially copyable values is well defined and pointers to objects that
 * existed before the receiver forked (function pointers, NautilusFunctionDefinitions)
 * stay valid. The only pointers that need translation are arena-allocated trace
 * objects (rebuilt in the receiver's arena) and Tag trie nodes (serialized as their
 * root-first address paths and re-interned into the receiver's trie, which preserves
 * tag identity for all future tag-map lookups).
 */

struct ByteWriter {
	std::vector<std::byte> buffer;

	template <typename T>
	    requires std::is_trivially_copyable_v<T>
	void write(const T& value) {
		writeBytes(&value, sizeof(T));
	}

	void writeBytes(const void* data, size_t size) {
		const auto* bytes = static_cast<const std::byte*>(data);
		buffer.insert(buffer.end(), bytes, bytes + size);
	}

	void writeString(const std::string& value) {
		write<uint64_t>(value.size());
		writeBytes(value.data(), value.size());
	}
};

struct ByteReader {
	std::span<const std::byte> data;
	size_t offset = 0;

	template <typename T>
	    requires std::is_trivially_copyable_v<T>
	T read() {
		T value;
		std::memcpy(&value, data.data() + offset, sizeof(T));
		offset += sizeof(T);
		return value;
	}

	void readBytes(void* out, size_t size) {
		std::memcpy(out, data.data() + offset, size);
		offset += size;
	}

	std::string readString() {
		auto size = read<uint64_t>();
		std::string value(size, '\0');
		readBytes(value.data(), size);
		return value;
	}
};

/// Serializes a snapshot as its tag's root-first address path plus the state hash.
void writeSnapshot(ByteWriter& writer, const Snapshot& snapshot);
/// Reads a snapshot, re-interning the tag path into @p tagRecorder's trie.
Snapshot readSnapshot(ByteReader& reader, TagRecorder& tagRecorder);

/// Serializes the complete grow-only state of @p trace (blocks, operations, tag map,
/// return refs, alloca table, value-ref counter).
void serializeTraceState(ByteWriter& writer, ExecutionTrace& trace);
/// Replaces the content of @p trace with the serialized state, allocating all blocks
/// and operations from the trace's own arena.
void deserializeTraceState(ByteReader& reader, ExecutionTrace& trace, TagRecorder& tagRecorder);

// --- Socket helpers (length-prefixed messages + SCM_RIGHTS fd transfer) ---

/// Writes a length-prefixed message; retries on EINTR/short writes. Throws on error.
void writeMessage(int fd, const std::vector<std::byte>& payload);
/// Reads a length-prefixed message. Throws on error or EOF.
std::vector<std::byte> readMessage(int fd);
/// Like readMessage but waits at most @p timeoutMs for the first byte; returns an
/// empty vector on timeout or EOF.
std::vector<std::byte> readMessageWithTimeout(int fd, int timeoutMs);

/// Transfers file descriptors over a UNIX socket (chunked below the SCM_RIGHTS limit).
void sendFds(int socketFd, const int* fds, size_t count);
/// Receives @p count file descriptors sent by sendFds.
void receiveFds(int socketFd, int* fds, size_t count);

} // namespace nautilus::tracing::serialization
