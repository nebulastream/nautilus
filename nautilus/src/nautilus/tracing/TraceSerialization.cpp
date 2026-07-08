
#include "TraceSerialization.hpp"
#include "Block.hpp"
#include "TraceOperation.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <poll.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <unistd.h>

namespace nautilus::tracing::serialization {

void SnapshotEncoder::write(const Snapshot& snapshot) {
	// Tags are serialized as their full node-to-root address chain. A prefix-
	// deduplicating encoding (emit each trie edge once per payload) was measured
	// and rejected: the tag trie is built leaf-first from backtrace() output, so
	// chains of distinct operations share no nodes and the dedupe only added
	// per-node bookkeeping overhead.
	path_.clear();
	for (const Tag* node = snapshot.getTag(); node != nullptr && node->getParent() != nullptr;
	     node = node->getParent()) {
		path_.push_back(node->getContent());
	}
	std::reverse(path_.begin(), path_.end());
	writer_.write<uint8_t>(snapshot.getTag() != nullptr ? 1 : 0);
	writer_.write<uint64_t>(path_.size());
	writer_.writeBytes(path_.data(), path_.size() * sizeof(TagAddress));
	writer_.write<uint64_t>(snapshot.getStateHash());
}

Snapshot SnapshotDecoder::read() {
	auto hasTag = reader_.read<uint8_t>();
	auto pathSize = reader_.read<uint64_t>();
	path_.resize(pathSize);
	reader_.readBytes(path_.data(), pathSize * sizeof(TagAddress));
	auto stateHash = reader_.read<uint64_t>();
	Tag* tag = hasTag != 0 ? tagRecorder_->internTagPath(path_.data(), path_.size()) : nullptr;
	return {tag, stateHash};
}

namespace {

// InputVariant alternative indices; mirrors the variant declaration order in
// TraceOperation.hpp and is verified there via static_assert-like construction below.
enum class InputKind : uint8_t {
	ValueRef = 0,
	None = 1,
	Constant = 2,
	Block = 3,
	FunctionCall = 4,
	Probability = 5,
	Alloca = 6,
	IndirectCall = 7,
};

void writeConstantLiteral(ByteWriter& writer, const ConstantLiteral& literal) {
	writer.write<uint8_t>(static_cast<uint8_t>(literal.index()));
	std::visit([&](auto value) { writer.write(value); }, literal);
}

ConstantLiteral readConstantLiteral(ByteReader& reader) {
	auto index = reader.read<uint8_t>();
	switch (index) {
	case 0:
		return {reader.read<bool>()};
	case 1:
		return {reader.read<uint8_t>()};
	case 2:
		return {reader.read<uint16_t>()};
	case 3:
		return {reader.read<uint32_t>()};
	case 4:
		return {reader.read<uint64_t>()};
	case 5:
		return {reader.read<int8_t>()};
	case 6:
		return {reader.read<int16_t>()};
	case 7:
		return {reader.read<int32_t>()};
	case 8:
		return {reader.read<int64_t>()};
	case 9:
		return {reader.read<float>()};
	case 10:
		return {reader.read<double>()};
	case 11:
		return {reader.read<void*>()};
	default:
		throw RuntimeException("Invalid constant literal in serialized trace");
	}
}

void writeValueRefs(ByteWriter& writer, const std::vector<TypedValueRef>& refs) {
	writer.write<uint64_t>(refs.size());
	for (const auto& ref : refs) {
		writer.write<uint32_t>(ref.ref);
		writer.write<uint8_t>(static_cast<uint8_t>(ref.type));
	}
}

std::vector<TypedValueRef> readValueRefs(ByteReader& reader) {
	auto size = reader.read<uint64_t>();
	std::vector<TypedValueRef> refs;
	refs.reserve(size);
	for (uint64_t i = 0; i < size; i++) {
		auto ref = reader.read<uint32_t>();
		auto type = static_cast<Type>(reader.read<uint8_t>());
		refs.emplace_back(ref, type);
	}
	return refs;
}

void writeInput(ByteWriter& writer, const InputVariant& input) {
	writer.write<uint8_t>(static_cast<uint8_t>(input.index()));
	if (const auto* valueRef = std::get_if<TypedValueRef>(&input)) {
		writer.write<uint32_t>(valueRef->ref);
		writer.write<uint8_t>(static_cast<uint8_t>(valueRef->type));
	} else if (std::holds_alternative<None>(input)) {
		// no payload
	} else if (const auto* constant = std::get_if<ConstantLiteral>(&input)) {
		writeConstantLiteral(writer, *constant);
	} else if (const auto* blockRef = std::get_if<BlockRef*>(&input)) {
		writer.write<uint32_t>((*blockRef)->block);
		writeValueRefs(writer, (*blockRef)->arguments);
	} else if (const auto* call = std::get_if<FunctionCall*>(&input)) {
		writer.writeString((*call)->functionName);
		writer.writeString((*call)->mangledName);
		writer.write<void*>((*call)->ptr);
		writeValueRefs(writer, (*call)->arguments);
		writer.write<FunctionAttributes>((*call)->fnAttrs);
	} else if (const auto* probability = std::get_if<BranchProbability>(&input)) {
		writer.write<double>(*probability);
	} else if (const auto* allocaIndex = std::get_if<AllocaIndex>(&input)) {
		writer.write<AllocaIndex>(*allocaIndex);
	} else if (const auto* indirect = std::get_if<IndirectFunctionCall*>(&input)) {
		writer.write<uint32_t>((*indirect)->fnPtr.ref);
		writer.write<uint8_t>(static_cast<uint8_t>((*indirect)->fnPtr.type));
		writeValueRefs(writer, (*indirect)->arguments);
		writer.write<FunctionAttributes>((*indirect)->fnAttrs);
	} else {
		throw RuntimeException("Unhandled trace operation input variant during serialization");
	}
}

InputVariant readInput(ByteReader& reader, common::Arena& arena) {
	auto kind = static_cast<InputKind>(reader.read<uint8_t>());
	switch (kind) {
	case InputKind::ValueRef: {
		auto ref = reader.read<uint32_t>();
		auto type = static_cast<Type>(reader.read<uint8_t>());
		return TypedValueRef {ref, type};
	}
	case InputKind::None:
		return None {};
	case InputKind::Constant:
		return readConstantLiteral(reader);
	case InputKind::Block: {
		auto* blockRef = arena.create<BlockRef>(reader.read<uint32_t>());
		blockRef->arguments = readValueRefs(reader);
		return blockRef;
	}
	case InputKind::FunctionCall: {
		auto functionName = reader.readString();
		auto mangledName = reader.readString();
		auto* ptr = reader.read<void*>();
		auto arguments = readValueRefs(reader);
		auto fnAttrs = reader.read<FunctionAttributes>();
		return arena.create<FunctionCall>(FunctionCall {.functionName = std::move(functionName),
		                                                .mangledName = std::move(mangledName),
		                                                .ptr = ptr,
		                                                .arguments = std::move(arguments),
		                                                .fnAttrs = fnAttrs});
	}
	case InputKind::Probability:
		return BranchProbability {reader.read<double>()};
	case InputKind::Alloca:
		return reader.read<AllocaIndex>();
	case InputKind::IndirectCall: {
		auto ref = reader.read<uint32_t>();
		auto type = static_cast<Type>(reader.read<uint8_t>());
		auto arguments = readValueRefs(reader);
		auto fnAttrs = reader.read<FunctionAttributes>();
		return arena.create<IndirectFunctionCall>(
		    IndirectFunctionCall {.fnPtr = {ref, type}, .arguments = std::move(arguments), .fnAttrs = fnAttrs});
	}
	}
	throw RuntimeException("Invalid trace operation input variant in serialized trace");
}

/// Serializes a block's mutable content (everything except the blockId).
void writeBlockContent(ByteWriter& writer, SnapshotEncoder& snapshots, const Block& block) {
	writer.write<uint8_t>(static_cast<uint8_t>(block.type));
	writeValueRefs(writer, block.arguments);
	writer.write<uint64_t>(block.predecessors.size());
	writer.writeBytes(block.predecessors.data(), block.predecessors.size() * sizeof(uint32_t));
	writer.write<uint64_t>(block.operations.size());
	for (const auto* operation : block.operations) {
		writer.write<uint8_t>(static_cast<uint8_t>(operation->op));
		writer.write<uint8_t>(static_cast<uint8_t>(operation->resultType));
		writer.write<uint32_t>(operation->resultRef.ref);
		writer.write<uint8_t>(static_cast<uint8_t>(operation->resultRef.type));
		snapshots.write(operation->tag);
		writer.write<uint64_t>(operation->input.size());
		for (const auto& input : operation->input) {
			writeInput(writer, input);
		}
	}
}

/// Rebuilds a block's content in place, allocating fresh operations from @p arena.
/// Previously referenced operations are abandoned (arena-owned, reclaimed later).
void readBlockContentInto(ByteReader& reader, SnapshotDecoder& snapshots, Block& block, common::Arena& arena) {
	block.type = static_cast<Block::Type>(reader.read<uint8_t>());
	block.arguments = readValueRefs(reader);
	auto predecessorCount = reader.read<uint64_t>();
	block.predecessors.resize(predecessorCount);
	reader.readBytes(block.predecessors.data(), predecessorCount * sizeof(uint32_t));
	auto operationCount = reader.read<uint64_t>();
	block.operations.clear();
	block.operations.reserve(operationCount);
	for (uint64_t opIndex = 0; opIndex < operationCount; opIndex++) {
		auto op = static_cast<Op>(reader.read<uint8_t>());
		auto resultType = static_cast<Type>(reader.read<uint8_t>());
		auto resultRefId = reader.read<uint32_t>();
		auto resultRefType = static_cast<Type>(reader.read<uint8_t>());
		auto tag = snapshots.read();
		auto inputCount = reader.read<uint64_t>();
		InputVariant* inputs = detail::allocateInputArray(arena, inputCount);
		for (uint64_t i = 0; i < inputCount; i++) {
			::new (&inputs[i]) InputVariant(readInput(reader, arena));
		}
		auto* operation = arena.create<TraceOperation>(tag, op, resultType, TypedValueRef {resultRefId, resultRefType},
		                                               std::span<InputVariant> {inputs, inputCount});
		block.operations.push_back(operation);
	}
}

} // namespace

void serializeTraceState(ByteWriter& writer, ExecutionTrace& trace) {
	SnapshotEncoder snapshots {writer};
	auto& blocks = trace.getBlocks();
	writer.write<uint64_t>(blocks.size());
	for (const auto* block : blocks) {
		writer.write<uint32_t>(block->blockId);
		writeBlockContent(writer, snapshots, *block);
	}

	const auto returnRefs = trace.getReturn();
	writer.write<uint64_t>(returnRefs.size());
	writer.writeBytes(returnRefs.data(), returnRefs.size() * sizeof(operation_identifier));

	writer.write<uint64_t>(trace.allocaSpecs.size());
	writer.writeBytes(trace.allocaSpecs.data(), trace.allocaSpecs.size() * sizeof(AllocaSpec));

	writer.write<uint32_t>(trace.lastValueRef);

	writer.write<uint64_t>(trace.globalTagMap.size());
	for (const auto& [snapshot, identifier] : trace.globalTagMap) {
		snapshots.write(snapshot);
		writer.write<operation_identifier>(identifier);
	}
}

void deserializeTraceState(ByteReader& reader, ExecutionTrace& trace, TagRecorder& tagRecorder) {
	SnapshotDecoder snapshots {reader, tagRecorder};
	auto& arena = trace.getArena();
	// Old blocks and operations are simply abandoned; they are arena-owned and get
	// reclaimed with the arena after compilation.
	trace.blocks.clear();
	trace.returnRefs.clear();
	trace.globalTagMap.clear();
	trace.localTagMap.clear();
	trace.allocaSpecs.clear();

	auto blockCount = reader.read<uint64_t>();
	trace.blocks.reserve(blockCount);
	for (uint64_t blockIndex = 0; blockIndex < blockCount; blockIndex++) {
		auto blockId = reader.read<uint32_t>();
		auto* block = arena.create<Block>(blockId);
		readBlockContentInto(reader, snapshots, *block, arena);
		trace.blocks.push_back(block);
	}

	auto returnCount = reader.read<uint64_t>();
	trace.returnRefs.resize(returnCount);
	reader.readBytes(trace.returnRefs.data(), returnCount * sizeof(operation_identifier));

	auto allocaCount = reader.read<uint64_t>();
	trace.allocaSpecs.resize(allocaCount);
	reader.readBytes(trace.allocaSpecs.data(), allocaCount * sizeof(AllocaSpec));

	trace.lastValueRef = reader.read<uint32_t>();

	auto tagCount = reader.read<uint64_t>();
	trace.globalTagMap.reserve(tagCount);
	for (uint64_t i = 0; i < tagCount; i++) {
		auto snapshot = snapshots.read();
		auto identifier = reader.read<operation_identifier>();
		trace.globalTagMap.emplace(snapshot, identifier);
	}
}

void serializeTraceDelta(ByteWriter& writer, ExecutionTrace& trace, uint64_t sinceEpoch) {
	SnapshotEncoder snapshots {writer};
	writer.write<uint64_t>(trace.mutationEpoch);
	writer.write<uint32_t>(trace.lastValueRef);

	// Small always-shipped state: return refs are updated in place by control-flow
	// merges and the alloca table has no per-entry epoch; both stay tiny.
	const auto returnRefs = trace.getReturn();
	writer.write<uint64_t>(returnRefs.size());
	writer.writeBytes(returnRefs.data(), returnRefs.size() * sizeof(operation_identifier));
	writer.write<uint64_t>(trace.allocaSpecs.size());
	writer.writeBytes(trace.allocaSpecs.data(), trace.allocaSpecs.size() * sizeof(AllocaSpec));

	// Dirty blocks (shipped whole, keyed by blockId; includes blocks created after
	// sinceEpoch, whose creation touched them).
	auto& blocks = trace.getBlocks();
	writer.write<uint64_t>(blocks.size());
	uint64_t dirtyCount = 0;
	for (const auto* block : blocks) {
		if (block->lastModifiedEpoch > sinceEpoch) {
			dirtyCount++;
		}
	}
	writer.write<uint64_t>(dirtyCount);
	for (const auto* block : blocks) {
		if (block->lastModifiedEpoch <= sinceEpoch) {
			continue;
		}
		writer.write<uint32_t>(block->blockId);
		writer.write<uint64_t>(block->lastModifiedEpoch);
		writeBlockContent(writer, snapshots, *block);
	}

	// Tag-journal suffix. The journal is in strictly increasing epoch order.
	const auto& journal = trace.tagJournal;
	auto firstNewer =
	    std::lower_bound(journal.begin(), journal.end(), sinceEpoch,
	                     [](const ExecutionTrace::TagMapWrite& write, uint64_t epoch) { return write.epoch <= epoch; });
	// Last-wins dedupe: control-flow merges rewrite the tag entries of every moved
	// operation, so the raw suffix repeats the same keys many times (O(paths x ops)
	// on merge-heavy traces). The receiver only needs each key's final position.
	// Keeping the last write per key is epoch-sound: that write carries the key's
	// maximum epoch, so any later re-delta with a lower cutoff still includes it.
	std::unordered_map<Snapshot, const ExecutionTrace::TagMapWrite*> lastWrites;
	for (auto it = firstNewer; it != journal.end(); ++it) {
		lastWrites[it->snapshot] = &*it;
	}
	std::vector<const ExecutionTrace::TagMapWrite*> suffix;
	suffix.reserve(lastWrites.size());
	for (const auto& [snapshot, write] : lastWrites) {
		suffix.push_back(write);
	}
	// Receivers append these to their own (epoch-sorted) journal, so keep order.
	std::sort(
	    suffix.begin(), suffix.end(),
	    [](const ExecutionTrace::TagMapWrite* a, const ExecutionTrace::TagMapWrite* b) { return a->epoch < b->epoch; });
	writer.write<uint64_t>(suffix.size());
	for (const auto* write : suffix) {
		snapshots.write(write->snapshot);
		writer.write<operation_identifier>(write->identifier);
		writer.write<uint64_t>(write->epoch);
	}
}

void applyTraceDelta(ByteReader& reader, ExecutionTrace& trace, TagRecorder& tagRecorder) {
	SnapshotDecoder snapshots {reader, tagRecorder};
	auto& arena = trace.getArena();

	trace.mutationEpoch = reader.read<uint64_t>();
	trace.lastValueRef = reader.read<uint32_t>();

	auto returnCount = reader.read<uint64_t>();
	trace.returnRefs.resize(returnCount);
	reader.readBytes(trace.returnRefs.data(), returnCount * sizeof(operation_identifier));
	auto allocaCount = reader.read<uint64_t>();
	trace.allocaSpecs.resize(allocaCount);
	reader.readBytes(trace.allocaSpecs.data(), allocaCount * sizeof(AllocaSpec));

	auto totalBlockCount = reader.read<uint64_t>();
	while (trace.blocks.size() < totalBlockCount) {
		auto* block = arena.create<Block>(static_cast<uint32_t>(trace.blocks.size()));
		trace.blocks.push_back(block);
	}
	auto dirtyCount = reader.read<uint64_t>();
	for (uint64_t i = 0; i < dirtyCount; i++) {
		auto blockId = reader.read<uint32_t>();
		auto epoch = reader.read<uint64_t>();
		if (blockId >= trace.blocks.size()) {
			throw RuntimeException("forkTracing: delta references an unknown block");
		}
		auto& block = *trace.blocks[blockId];
		readBlockContentInto(reader, snapshots, block, arena);
		block.lastModifiedEpoch = epoch;
	}

	// The resumed suffix is a fresh path: apply the journal suffix into the global
	// map only and extend the receiver's own journal so it can later produce deltas
	// for continuations that forked before these writes.
	trace.localTagMap.clear();
	auto journalCount = reader.read<uint64_t>();
	trace.tagJournal.reserve(trace.tagJournal.size() + journalCount);
	for (uint64_t i = 0; i < journalCount; i++) {
		auto snapshot = snapshots.read();
		auto identifier = reader.read<operation_identifier>();
		auto epoch = reader.read<uint64_t>();
		trace.globalTagMap[snapshot] = identifier;
		trace.tagJournal.push_back({snapshot, identifier, epoch});
	}
}

// --- Socket helpers ---

namespace {

void writeAll(int fd, const void* data, size_t size) {
	const auto* bytes = static_cast<const std::byte*>(data);
	size_t written = 0;
	while (written < size) {
		auto result = send(fd, bytes + written, size - written, MSG_NOSIGNAL);
		if (result < 0) {
			if (errno == EINTR) {
				continue;
			}
			throw RuntimeException("forkTracing: failed to write to handoff socket: " +
			                       std::string(std::strerror(errno)));
		}
		written += static_cast<size_t>(result);
	}
}

void readAll(int fd, void* data, size_t size) {
	auto* bytes = static_cast<std::byte*>(data);
	size_t consumed = 0;
	while (consumed < size) {
		auto result = recv(fd, bytes + consumed, size - consumed, 0);
		if (result < 0) {
			if (errno == EINTR) {
				continue;
			}
			throw RuntimeException("forkTracing: failed to read from handoff socket: " +
			                       std::string(std::strerror(errno)));
		}
		if (result == 0) {
			throw RuntimeException("forkTracing: handoff socket closed unexpectedly");
		}
		consumed += static_cast<size_t>(result);
	}
}

} // namespace

void writeMessage(int fd, const std::vector<std::byte>& payload) {
	uint64_t size = payload.size();
	writeAll(fd, &size, sizeof(size));
	writeAll(fd, payload.data(), payload.size());
}

std::vector<std::byte> readMessage(int fd) {
	uint64_t size = 0;
	readAll(fd, &size, sizeof(size));
	std::vector<std::byte> payload(size);
	readAll(fd, payload.data(), size);
	return payload;
}

std::vector<std::byte> readMessageWithTimeout(int fd, int timeoutMs) {
	pollfd pollDescriptor {fd, POLLIN, 0};
	while (true) {
		auto result = poll(&pollDescriptor, 1, timeoutMs);
		if (result < 0 && errno == EINTR) {
			continue;
		}
		if (result <= 0) {
			return {};
		}
		break;
	}
	uint64_t size = 0;
	auto peeked = recv(fd, &size, sizeof(size), MSG_PEEK);
	if (peeked == 0) {
		return {}; // all writers exited without a result
	}
	readAll(fd, &size, sizeof(size));
	std::vector<std::byte> payload(size);
	readAll(fd, payload.data(), size);
	return payload;
}

// Small SCM_RIGHTS chunks: Linux caps a single message at SCM_MAX_FD (253) but other
// kernels are less forgiving with large ancillary payloads; 16 per message is safely
// below every platform limit at negligible cost.
constexpr size_t FD_CHUNK_SIZE = 16;

void sendFds(int socketFd, const int* fds, size_t count) {
	size_t sent = 0;
	while (sent < count) {
		auto chunk = std::min(FD_CHUNK_SIZE, count - sent);
		char token = 'f';
		iovec io {&token, 1};
		alignas(cmsghdr) char control[CMSG_SPACE(FD_CHUNK_SIZE * sizeof(int))];
		std::memset(control, 0, sizeof(control));
		msghdr message {};
		message.msg_iov = &io;
		message.msg_iovlen = 1;
		message.msg_control = control;
		message.msg_controllen = CMSG_SPACE(chunk * sizeof(int));
		auto* header = CMSG_FIRSTHDR(&message);
		header->cmsg_level = SOL_SOCKET;
		header->cmsg_type = SCM_RIGHTS;
		header->cmsg_len = CMSG_LEN(chunk * sizeof(int));
		std::memcpy(CMSG_DATA(header), fds + sent, chunk * sizeof(int));
		// Ancillary sends do not block when the peer's receive buffer is full; some
		// kernels (xnu) report EMSGSIZE instead. Give the receiver time to drain.
		int backoffBudget = 5000; // x 1ms = 5s
		while (sendmsg(socketFd, &message, MSG_NOSIGNAL) < 0) {
			if (errno == EINTR) {
				continue;
			}
			if ((errno == EMSGSIZE || errno == ENOBUFS || errno == EAGAIN) && backoffBudget-- > 0) {
				usleep(1000);
				continue;
			}
			throw RuntimeException("forkTracing: failed to transfer continuation descriptors: " +
			                       std::string(std::strerror(errno)));
		}
		sent += chunk;
	}
}

void receiveFds(int socketFd, int* fds, size_t count) {
	size_t received = 0;
	while (received < count) {
		char token = 0;
		iovec io {&token, 1};
		alignas(cmsghdr) char control[CMSG_SPACE(FD_CHUNK_SIZE * sizeof(int))];
		msghdr message {};
		message.msg_iov = &io;
		message.msg_iovlen = 1;
		message.msg_control = control;
		message.msg_controllen = sizeof(control);
		ssize_t result;
		while ((result = recvmsg(socketFd, &message, 0)) < 0) {
			if (errno != EINTR) {
				throw RuntimeException("forkTracing: failed to receive continuation descriptors: " +
				                       std::string(std::strerror(errno)));
			}
		}
		if (result == 0) {
			throw RuntimeException("forkTracing: handoff socket closed during descriptor transfer");
		}
		if ((message.msg_flags & MSG_CTRUNC) != 0) {
			throw RuntimeException("forkTracing: descriptor transfer message was truncated");
		}
		auto* header = CMSG_FIRSTHDR(&message);
		if (header == nullptr || header->cmsg_level != SOL_SOCKET || header->cmsg_type != SCM_RIGHTS) {
			throw RuntimeException("forkTracing: malformed descriptor transfer message");
		}
		// Derive the actual descriptor count from the message instead of assuming the
		// sender's chunking; kernels may deliver fewer per message than were sent.
		auto chunk = (header->cmsg_len - CMSG_LEN(0)) / sizeof(int);
		if (chunk == 0 || received + chunk > count) {
			throw RuntimeException("forkTracing: unexpected descriptor count in transfer message");
		}
		std::memcpy(fds + received, CMSG_DATA(header), chunk * sizeof(int));
		received += chunk;
	}
}

SharedHandoffRegion::SharedHandoffRegion(size_t capacity) : capacity_(capacity) {
	// MAP_SHARED so forked processes see each other's writes; anonymous, so pages
	// are committed only up to the payload high-water mark, not the full capacity.
	mapping_ = mmap(nullptr, capacity, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (mapping_ == MAP_FAILED) {
		mapping_ = nullptr;
		throw RuntimeException("forkTracing: failed to allocate the shared handoff region");
	}
}

SharedHandoffRegion::~SharedHandoffRegion() {
	if (mapping_ != nullptr) {
		munmap(mapping_, capacity_);
	}
}

} // namespace nautilus::tracing::serialization
