
#pragma once

#include <cstdint>
#include <stdexcept>
#include <sys/mman.h>
#include <unistd.h>

namespace nautilus::tracing {

/**
 * @brief Exception thrown when the shared trie runs out of capacity.
 * The caller should catch this, allocate a larger shared memory region,
 * and restart tracing.
 */
class TrieCapacityExceeded : public std::runtime_error {
public:
	explicit TrieCapacityExceeded(size_t required, size_t capacity)
	    : std::runtime_error("Trie capacity exceeded"), requiredCapacity(required), currentCapacity(capacity) {
	}

	size_t requiredCapacity;
	size_t currentCapacity;
};

/**
 * @brief Index type for referencing nodes in the shared trie.
 * Using uint32_t allows up to 4 billion nodes while keeping memory compact.
 */
using TrieIndex = uint32_t;

/**
 * @brief Sentinel value indicating null/invalid index.
 */
constexpr TrieIndex TRIE_NULL = UINT32_MAX;

/**
 * @brief A node in the shared memory trie.
 * Uses indices instead of pointers for shared memory compatibility.
 *
 * Children are stored as a linked list using firstChild/nextSibling.
 * This avoids variable-size data within nodes.
 */
template <typename T>
struct SharedTrieNode {
	T content;
	TrieIndex firstChild;  // Index of first child node
	TrieIndex nextSibling; // Index of next sibling (for iterating children)

	SharedTrieNode() : content {}, firstChild(TRIE_NULL), nextSibling(TRIE_NULL) {
	}

	explicit SharedTrieNode(T value) : content(value), firstChild(TRIE_NULL), nextSibling(TRIE_NULL) {
	}
};

/**
 * @brief Header structure stored at the beginning of the shared memory region.
 * Contains metadata about the trie state.
 */
struct SharedTrieHeader {
	uint32_t magic;      // Magic number for validation
	uint32_t version;    // Version for compatibility checking
	pid_t creatorPid;    // PID of process that created this trie
	size_t memorySize;   // Total size of shared memory region
	size_t capacity;     // Maximum number of nodes
	size_t size;         // Current number of allocated nodes
	TrieIndex rootIndex; // Index of the root node (always 0 after init)

	static constexpr uint32_t MAGIC = 0x54524945; // "TRIE"
	static constexpr uint32_t VERSION = 1;
};

/**
 * @brief A trie data structure backed by shared memory.
 *
 * Uses a flat array of nodes with indices instead of pointers.
 * The memory layout is:
 *   [SharedTrieHeader][SharedTrieNode[0]][SharedTrieNode[1]]...
 *
 * Automatically allocates shared memory on creation and frees it
 * in the destructor (only in the process that created it).
 *
 * @tparam T The type of values stored in the trie (e.g., TagAddress)
 */
template <typename T>
class SharedTrie {
public:
	using Node = SharedTrieNode<T>;

	/**
	 * @brief Calculate the required memory size for a trie with given capacity.
	 * @param capacity Maximum number of nodes
	 * @return Size in bytes
	 */
	static size_t requiredMemorySize(size_t capacity) {
		return sizeof(SharedTrieHeader) + capacity * sizeof(Node);
	}

	/**
	 * @brief Create a new trie with shared memory allocation.
	 * @param capacity Maximum number of nodes
	 * @return SharedTrie instance that owns the shared memory
	 */
	static SharedTrie create(size_t capacity) {
		size_t memSize = requiredMemorySize(capacity);

		void* memory = mmap(nullptr, memSize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
		if (memory == MAP_FAILED) {
			throw std::runtime_error("Failed to allocate shared memory for SharedTrie");
		}

		auto* header = static_cast<SharedTrieHeader*>(memory);
		header->magic = SharedTrieHeader::MAGIC;
		header->version = SharedTrieHeader::VERSION;
		header->creatorPid = getpid();
		header->memorySize = memSize;
		header->capacity = capacity;
		header->size = 1; // Root node
		header->rootIndex = 0;

		auto* nodes = reinterpret_cast<Node*>(static_cast<char*>(memory) + sizeof(SharedTrieHeader));

		// Initialize root node
		new (&nodes[0]) Node();

		return SharedTrie(header, nodes);
	}

	// Delete copy operations (shared memory ownership)
	SharedTrie(const SharedTrie&) = delete;
	SharedTrie& operator=(const SharedTrie&) = delete;

	// Move operations
	SharedTrie(SharedTrie&& other) noexcept : header_(other.header_), nodes_(other.nodes_) {
		other.header_ = nullptr;
		other.nodes_ = nullptr;
	}

	SharedTrie& operator=(SharedTrie&& other) noexcept {
		if (this != &other) {
			cleanup();
			header_ = other.header_;
			nodes_ = other.nodes_;
			other.header_ = nullptr;
			other.nodes_ = nullptr;
		}
		return *this;
	}

	/**
	 * @brief Destructor. Frees shared memory only if called from creator process.
	 */
	~SharedTrie() {
		cleanup();
	}

	/**
	 * @brief Get the root node index.
	 */
	TrieIndex root() const {
		return header_->rootIndex;
	}

	/**
	 * @brief Get a node by index.
	 */
	Node& operator[](TrieIndex index) {
		return nodes_[index];
	}

	const Node& operator[](TrieIndex index) const {
		return nodes_[index];
	}

	/**
	 * @brief Append a value as a child of the given node.
	 * If a child with this value already exists, returns its index.
	 * Otherwise, creates a new node.
	 *
	 * @param parentIndex Index of the parent node
	 * @param value Value to append
	 * @return Index of the child node (existing or new)
	 * @throws TrieCapacityExceeded if no space for new node
	 */
	TrieIndex append(TrieIndex parentIndex, const T& value) {
		Node& parent = nodes_[parentIndex];

		// Search for existing child with this value
		TrieIndex childIndex = parent.firstChild;
		TrieIndex prevIndex = TRIE_NULL;

		while (childIndex != TRIE_NULL) {
			if (nodes_[childIndex].content == value) {
				return childIndex; // Found existing child
			}
			prevIndex = childIndex;
			childIndex = nodes_[childIndex].nextSibling;
		}

		// Need to allocate new node
		if (header_->size >= header_->capacity) {
			throw TrieCapacityExceeded(header_->size + 1, header_->capacity);
		}

		TrieIndex newIndex = static_cast<TrieIndex>(header_->size++);
		new (&nodes_[newIndex]) Node(value);

		// Link into parent's child list
		if (prevIndex == TRIE_NULL) {
			parent.firstChild = newIndex;
		} else {
			nodes_[prevIndex].nextSibling = newIndex;
		}

		return newIndex;
	}

	/**
	 * @brief Insert a path (sequence of values) into the trie.
	 * Starts from root and follows/creates nodes for each value in the path.
	 *
	 * @param begin Iterator to first value
	 * @param end Iterator past last value
	 * @return Index of the final node in the path (represents the full path)
	 * @throws TrieCapacityExceeded if no space for new nodes
	 */
	template <typename Iterator>
	TrieIndex insert(Iterator begin, Iterator end) {
		TrieIndex current = root();
		for (auto it = begin; it != end; ++it) {
			current = append(current, *it);
		}
		return current;
	}

	/**
	 * @brief Insert a path from a container (vector, array, etc).
	 *
	 * @param path Container with values
	 * @return Index of the final node (represents the full path)
	 */
	template <typename Container>
	TrieIndex insert(const Container& path) {
		return insert(std::begin(path), std::end(path));
	}

	/**
	 * @brief Insert a path from initializer list.
	 *
	 * @param path Initializer list of values
	 * @return Index of the final node
	 */
	TrieIndex insert(std::initializer_list<T> path) {
		return insert(path.begin(), path.end());
	}

	/**
	 * @brief Get the current number of nodes.
	 */
	size_t size() const {
		return header_->size;
	}

	/**
	 * @brief Get the maximum capacity.
	 */
	size_t capacity() const {
		return header_->capacity;
	}

	/**
	 * @brief Reset the trie to initial state (only root node).
	 * Useful when restarting tracing.
	 */
	void reset() {
		header_->size = 1;
		nodes_[0] = Node();
	}

private:
	SharedTrie(SharedTrieHeader* header, Node* nodes) : header_(header), nodes_(nodes) {
	}

	void cleanup() {
		if (header_ != nullptr && getpid() == header_->creatorPid) {
			munmap(header_, header_->memorySize);
		}
		header_ = nullptr;
		nodes_ = nullptr;
	}

	SharedTrieHeader* header_;
	Node* nodes_;
};

} // namespace nautilus::tracing
