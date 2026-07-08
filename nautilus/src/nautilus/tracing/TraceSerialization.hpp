
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

/**
 * @brief Serializes the tags of a payload's snapshots as full root-first address
 * paths (re-interned by the decoder via TagRecorder::internTagPath).
 *
 * Note for future optimization attempts: a prefix-deduplicating encoding (emit
 * each trie edge once per payload, reference nodes by id) was implemented and
 * benchmarked - and REVERTED. The tag trie is built leaf-first from backtrace()
 * output (TagRecorder::createReferenceTagBacktrace inserts the innermost frame
 * as the root's child), so the chains of distinct operations share no trie
 * nodes and there are no prefixes to deduplicate; the id bookkeeping only added
 * encoder/decoder overhead. Prefix sharing would require flipping the trie to
 * root-first order in TagRecorder, which affects every tracer's hot path.
 */
class SnapshotEncoder {
public:
	explicit SnapshotEncoder(ByteWriter& writer) : writer_(writer) {
	}
	void write(const Snapshot& snapshot);

private:
	ByteWriter& writer_;
	std::vector<TagAddress> path_; // scratch, reused across snapshots
};

/// Decodes snapshots written by SnapshotEncoder, re-interning tag paths into
/// @p tagRecorder (node identity is preserved across payloads).
class SnapshotDecoder {
public:
	SnapshotDecoder(ByteReader& reader, TagRecorder& tagRecorder) : reader_(reader), tagRecorder_(&tagRecorder) {
	}
	Snapshot read();

private:
	ByteReader& reader_;
	TagRecorder* tagRecorder_;
	std::vector<TagAddress> path_; // scratch, reused across snapshots
};

/// Serializes the complete grow-only state of @p trace (blocks, operations, tag map,
/// return refs, alloca table, value-ref counter).
void serializeTraceState(ByteWriter& writer, ExecutionTrace& trace);
/// Replaces the content of @p trace with the serialized state, allocating all blocks
/// and operations from the trace's own arena.
void deserializeTraceState(ByteReader& reader, ExecutionTrace& trace, TagRecorder& tagRecorder);

/**
 * @brief Serializes only the trace mutations newer than @p sinceEpoch: blocks whose
 * Block::lastModifiedEpoch is newer, the (last-wins deduplicated) tag-journal suffix,
 * and the small always-shipped scalars (value-ref counter, return refs, alloca table).
 *
 * Requires ExecutionTrace::enableDeltaTracking() to have been active for the whole
 * lifetime of @p trace. A continuation that forked at epoch E has state exactly equal
 * to the timeline at E (it performs no trace writes while suspended), so applying the
 * delta > E on top reproduces the sender's full state.
 */
void serializeTraceDelta(ByteWriter& writer, ExecutionTrace& trace, uint64_t sinceEpoch);
/// Applies a delta produced by serializeTraceDelta on top of the receiver's own state,
/// re-interning tags into @p tagRecorder and extending the receiver's journal so it can
/// itself produce deltas for older continuations later.
void applyTraceDelta(ByteReader& reader, ExecutionTrace& trace, TagRecorder& tagRecorder);

/**
 * @brief Anonymous MAP_SHARED region inherited (at the same address) by every process
 * forked after its creation. The fork tracer stages handoff payloads here so the bytes
 * are written once by the sender and read in place by the receiver, instead of being
 * copied twice through kernel socket buffers; the socket message that follows carries
 * only the payload size and acts as the synchronization fence. Handoffs are strictly
 * sequential (exactly one process in the worker tree runs at any moment), so a single
 * region without further locking is race-free.
 */
class SharedHandoffRegion {
public:
	explicit SharedHandoffRegion(size_t capacity);
	~SharedHandoffRegion();
	SharedHandoffRegion(const SharedHandoffRegion&) = delete;
	SharedHandoffRegion& operator=(const SharedHandoffRegion&) = delete;
	SharedHandoffRegion(SharedHandoffRegion&&) = delete;
	SharedHandoffRegion& operator=(SharedHandoffRegion&&) = delete;

	std::byte* data() {
		return static_cast<std::byte*>(mapping_);
	}
	size_t capacity() const {
		return capacity_;
	}

private:
	void* mapping_ = nullptr;
	size_t capacity_ = 0;
};

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
