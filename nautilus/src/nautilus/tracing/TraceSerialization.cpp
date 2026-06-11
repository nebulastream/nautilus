
#include "TraceSerialization.hpp"
#include "Block.hpp"
#include "TraceOperation.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

namespace nautilus::tracing::serialization {

void writeSnapshot(ByteWriter& writer, const Snapshot& snapshot) {
	std::vector<TagAddress> path;
	// Walk to the trie root; the root is a sentinel (parent == nullptr) and carries
	// no address, so it is excluded from the path.
	for (const Tag* node = snapshot.getTag(); node != nullptr && node->getParent() != nullptr;
	     node = node->getParent()) {
		path.push_back(node->getContent());
	}
	std::reverse(path.begin(), path.end());
	writer.write<uint8_t>(snapshot.getTag() != nullptr ? 1 : 0);
	writer.write<uint64_t>(path.size());
	writer.writeBytes(path.data(), path.size() * sizeof(TagAddress));
	writer.write<uint64_t>(snapshot.getStateHash());
}

Snapshot readSnapshot(ByteReader& reader, TagRecorder& tagRecorder) {
	auto hasTag = reader.read<uint8_t>();
	auto pathSize = reader.read<uint64_t>();
	std::vector<TagAddress> path(pathSize);
	reader.readBytes(path.data(), pathSize * sizeof(TagAddress));
	auto stateHash = reader.read<uint64_t>();
	Tag* tag = hasTag != 0 ? tagRecorder.internTagPath(path.data(), path.size()) : nullptr;
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

} // namespace

void serializeTraceState(ByteWriter& writer, ExecutionTrace& trace) {
	auto& blocks = trace.getBlocks();
	writer.write<uint64_t>(blocks.size());
	for (const auto* block : blocks) {
		writer.write<uint32_t>(block->blockId);
		writer.write<uint8_t>(static_cast<uint8_t>(block->type));
		writeValueRefs(writer, block->arguments);
		writer.write<uint64_t>(block->predecessors.size());
		writer.writeBytes(block->predecessors.data(), block->predecessors.size() * sizeof(uint32_t));
		writer.write<uint64_t>(block->operations.size());
		for (const auto* operation : block->operations) {
			writer.write<uint8_t>(static_cast<uint8_t>(operation->op));
			writer.write<uint8_t>(static_cast<uint8_t>(operation->resultType));
			writer.write<uint32_t>(operation->resultRef.ref);
			writer.write<uint8_t>(static_cast<uint8_t>(operation->resultRef.type));
			writeSnapshot(writer, operation->tag);
			writer.write<uint64_t>(operation->input.size());
			for (const auto& input : operation->input) {
				writeInput(writer, input);
			}
		}
	}

	const auto returnRefs = trace.getReturn();
	writer.write<uint64_t>(returnRefs.size());
	writer.writeBytes(returnRefs.data(), returnRefs.size() * sizeof(operation_identifier));

	writer.write<uint64_t>(trace.allocaSpecs.size());
	writer.writeBytes(trace.allocaSpecs.data(), trace.allocaSpecs.size() * sizeof(AllocaSpec));

	writer.write<uint32_t>(trace.lastValueRef);

	writer.write<uint64_t>(trace.globalTagMap.size());
	for (const auto& [snapshot, identifier] : trace.globalTagMap) {
		writeSnapshot(writer, snapshot);
		writer.write<operation_identifier>(identifier);
	}
}

void deserializeTraceState(ByteReader& reader, ExecutionTrace& trace, TagRecorder& tagRecorder) {
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
		block->type = static_cast<Block::Type>(reader.read<uint8_t>());
		block->arguments = readValueRefs(reader);
		auto predecessorCount = reader.read<uint64_t>();
		block->predecessors.resize(predecessorCount);
		reader.readBytes(block->predecessors.data(), predecessorCount * sizeof(uint32_t));
		auto operationCount = reader.read<uint64_t>();
		block->operations.reserve(operationCount);
		for (uint64_t opIndex = 0; opIndex < operationCount; opIndex++) {
			auto op = static_cast<Op>(reader.read<uint8_t>());
			auto resultType = static_cast<Type>(reader.read<uint8_t>());
			auto resultRefId = reader.read<uint32_t>();
			auto resultRefType = static_cast<Type>(reader.read<uint8_t>());
			auto tag = readSnapshot(reader, tagRecorder);
			auto inputCount = reader.read<uint64_t>();
			InputVariant* inputs = detail::allocateInputArray(arena, inputCount);
			for (uint64_t i = 0; i < inputCount; i++) {
				::new (&inputs[i]) InputVariant(readInput(reader, arena));
			}
			auto* operation =
			    arena.create<TraceOperation>(tag, op, resultType, TypedValueRef {resultRefId, resultRefType},
			                                 std::span<InputVariant> {inputs, inputCount});
			block->operations.push_back(operation);
		}
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
		auto snapshot = readSnapshot(reader, tagRecorder);
		auto identifier = reader.read<operation_identifier>();
		trace.globalTagMap.emplace(snapshot, identifier);
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

void sendFds(int socketFd, const int* fds, size_t count) {
	// Stay well below the kernel's SCM_RIGHTS per-message limit (SCM_MAX_FD = 253).
	constexpr size_t chunkSize = 32;
	size_t sent = 0;
	while (sent < count) {
		auto chunk = std::min(chunkSize, count - sent);
		char token = 'f';
		iovec io {&token, 1};
		alignas(cmsghdr) char control[CMSG_SPACE(chunkSize * sizeof(int))];
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
		while (sendmsg(socketFd, &message, MSG_NOSIGNAL) < 0) {
			if (errno != EINTR) {
				throw RuntimeException("forkTracing: failed to transfer continuation descriptors: " +
				                       std::string(std::strerror(errno)));
			}
		}
		sent += chunk;
	}
}

void receiveFds(int socketFd, int* fds, size_t count) {
	constexpr size_t chunkSize = 32;
	size_t received = 0;
	while (received < count) {
		auto chunk = std::min(chunkSize, count - received);
		char token = 0;
		iovec io {&token, 1};
		alignas(cmsghdr) char control[CMSG_SPACE(chunkSize * sizeof(int))];
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
		auto* header = CMSG_FIRSTHDR(&message);
		if (header == nullptr || header->cmsg_level != SOL_SOCKET || header->cmsg_type != SCM_RIGHTS) {
			throw RuntimeException("forkTracing: malformed descriptor transfer message");
		}
		std::memcpy(fds + received, CMSG_DATA(header), chunk * sizeof(int));
		received += chunk;
	}
}

} // namespace nautilus::tracing::serialization
