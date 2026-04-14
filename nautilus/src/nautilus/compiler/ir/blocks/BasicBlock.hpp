
#pragma once

#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <compare>
#include <cstdint>
#include <memory>
#include <vector>

namespace nautilus::compiler::ir {

/**
 * @brief Strong typedef for a BasicBlock id.
 *
 * Mirrors `OperationIdentifier`: a `constexpr` value type wrapping a
 * `uint32_t`, so passing it by value is trivial and raw integer ids
 * cannot be accidentally mixed with op ids in containers or function
 * signatures. Implicit construction from `uint32_t` is allowed so
 * call sites that already hold a raw block-id (e.g. the tracing layer)
 * don't need per-site conversions.
 */
class BlockIdentifier {
public:
	constexpr BlockIdentifier(uint32_t id) : id(id) {
	}

	constexpr auto operator<=>(const BlockIdentifier&) const = default;
	constexpr bool operator==(const BlockIdentifier&) const = default;

	[[nodiscard]] constexpr uint32_t getId() const {
		return id;
	}

private:
	uint32_t id;
};

class BasicBlock {
public:
	/**
	 * @brief BasicBlock used for control flow in NES IR
	 * @param Operations: A list of Operations that are executed in the BasicBlock.
	 * @param nextBlocks : The BasicBlock that is next in the control flow of the execution.
	 */
	explicit BasicBlock(BlockIdentifier identifier, std::vector<std::unique_ptr<BasicBlockArgument>> arguments);

	virtual ~BasicBlock();

	[[nodiscard]] BlockIdentifier getIdentifier() const;

	[[nodiscard]] const std::vector<std::unique_ptr<Operation>>& getOperations() const;

	[[nodiscard]] Operation* getOperationAt(size_t index);

	[[nodiscard]] Operation* getTerminatorOp();

	[[nodiscard]] const std::vector<std::unique_ptr<BasicBlockArgument>>& getArguments() const;

	template <typename T, typename... Args>
	T* addOperation(Args&&... operation) {
		auto op = std::make_unique<T>(std::forward<Args>(operation)...);
		auto& ref = this->operations.emplace_back(std::move(op));
		return as<T>(ref);
	}

	BasicBlock* addOperation(std::unique_ptr<Operation> operation);

	BasicBlock* addNextBlock(BasicBlock* nextBlock);

	void addNextBlock(BasicBlock* nextBlock, const std::vector<Operation*>& inputArguments);

	BasicBlock* addTrueBlock(BasicBlock* thenBlock);

	BasicBlock* addFalseBlock(BasicBlock* elseBlock);

	void replaceOperation(size_t operationIndex, Operation* operation);

	void removeOperation(Operation* operation);

	void removeArgument(std::shared_ptr<BasicBlockArgument> argument);

	void addOperationBefore(Operation* before, std::unique_ptr<Operation>& operation);

	uint64_t getIndexOfArgument(Operation* arg);

	void replaceTerminatorOperation(Operation* newTerminatorOperation);

	[[nodiscard]] std::pair<const BasicBlock*, const BasicBlock*> getNextBlocks();

private:
	BlockIdentifier identifier;
	std::vector<std::unique_ptr<Operation>> operations;
	std::vector<std::unique_ptr<BasicBlockArgument>> arguments;
};

using BasicBlockPtr = std::shared_ptr<BasicBlock>;

} // namespace nautilus::compiler::ir

namespace std {
template <>
struct hash<nautilus::compiler::ir::BlockIdentifier> {
	std::size_t operator()(nautilus::compiler::ir::BlockIdentifier k) const noexcept {
		return std::hash<uint32_t> {}(k.getId());
	}
};
} // namespace std
