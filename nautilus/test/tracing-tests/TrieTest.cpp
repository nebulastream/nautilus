#include "nautilus/common/Arena.hpp"
#include "nautilus/tracing/tag/Trie.hpp"
#include <catch2/catch_test_macros.hpp>
#include <set>
#include <vector>

namespace nautilus::tracing {

TEST_CASE("TrieNode::append deduplicates repeated values under the same parent", "[Trie]") {
	common::Arena arena;
	TrieNode<uint64_t> root;

	uint64_t a = 1;
	uint64_t aAgain = 1;
	uint64_t b = 2;

	auto* first = root.append(a, arena);
	auto* second = root.append(aAgain, arena);
	auto* third = root.append(b, arena);

	REQUIRE(first == second);
	REQUIRE(first != third);
	REQUIRE(first->getContent() == 1);
	REQUIRE(third->getContent() == 2);
}

TEST_CASE("TrieNode::append builds a stable parent chain", "[Trie]") {
	common::Arena arena;
	TrieNode<uint64_t> root;

	uint64_t a = 10;
	uint64_t b = 20;

	auto* child = root.append(a, arena);
	auto* grandchild = child->append(b, arena);

	REQUIRE(grandchild->getParent() == child);
	REQUIRE(child->getParent() == &root);
	REQUIRE(root.getParent() == nullptr);
}

TEST_CASE("TrieNode::append grows past its initial inline capacity while keeping node identity", "[Trie]") {
	// Exercise the child-array growth path (capacity starts at 0 and doubles from
	// 4), verifying earlier-returned pointers stay valid and re-lookups still
	// resolve to the same node after growth reallocates the pointer array.
	common::Arena arena;
	TrieNode<uint64_t> root;

	constexpr int numChildren = 64;
	std::vector<TrieNode<uint64_t>*> nodes;
	for (uint64_t i = 0; i < numChildren; i++) {
		nodes.push_back(root.append(i, arena));
	}

	for (uint64_t i = 0; i < numChildren; i++) {
		auto* looked = root.append(i, arena);
		REQUIRE(looked == nodes[i]);
		REQUIRE(looked->getContent() == i);
	}
}

TEST_CASE("TrieNode tries sharing one arena do not cross-contaminate", "[Trie]") {
	// Mirrors TraceContext::startTrace, which builds one TagRecorder (and
	// therefore one trie) per traced function but hands all of them the same
	// compile-scoped Arena. Independent tries in that arena must stay distinct.
	common::Arena arena;
	TrieNode<uint64_t> rootOne;
	TrieNode<uint64_t> rootTwo;

	uint64_t a = 1;
	auto* childOfOne = rootOne.append(a, arena);
	auto* childOfTwo = rootTwo.append(a, arena);

	REQUIRE(childOfOne != childOfTwo);
	REQUIRE(childOfOne->getParent() == &rootOne);
	REQUIRE(childOfTwo->getParent() == &rootTwo);
}

} // namespace nautilus::tracing

namespace nautilus::tracing {

TEST_CASE("TrieNode::append switches to hashed children at the ninth child and keeps identity", "[Trie]") {
	common::Arena arena;
	TrieNode<uint64_t> root;
	std::vector<TrieNode<uint64_t>*> nodes;
	// code addresses are aligned: the hash must not depend on the low bits being varied
	for (uint64_t i = 0; i < 9; i++) {
		uint64_t address = 0x5555'0000'0000ull + i * 16;
		nodes.push_back(root.append(address, arena)); // the ninth switches to the table
	}
	for (uint64_t i = 0; i < nodes.size(); i++) {
		uint64_t address = 0x5555'0000'0000ull + i * 16;
		REQUIRE(root.append(address, arena) == nodes[i]);
	}
}

TEST_CASE("TrieNode::append with thousands of aligned children finds each one", "[Trie]") {
	common::Arena arena;
	TrieNode<uint64_t> root;
	constexpr uint64_t count = 5000;
	std::vector<TrieNode<uint64_t>*> nodes;
	for (uint64_t i = 0; i < count; i++) {
		uint64_t address = 0x7f00'0000'0000ull + i * 32;
		auto* node = root.append(address, arena);
		REQUIRE(node->getParent() == &root);
		nodes.push_back(node);
	}
	std::set<TrieNode<uint64_t>*> distinct(nodes.begin(), nodes.end());
	REQUIRE(distinct.size() == count);
	for (uint64_t i = 0; i < count; i++) {
		uint64_t address = 0x7f00'0000'0000ull + i * 32;
		REQUIRE(root.append(address, arena) == nodes[i]);
		REQUIRE(nodes[i]->getContent() == address);
	}
	// a child of a hashed child still chains up
	uint64_t below = 42;
	REQUIRE(nodes[1234]->append(below, arena)->getParent() == nodes[1234]);
}

} // namespace nautilus::tracing
