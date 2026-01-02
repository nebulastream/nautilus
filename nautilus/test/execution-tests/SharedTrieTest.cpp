
#include "nautilus/tracing/tag/SharedTrie.hpp"
#include <catch2/catch_test_macros.hpp>
#include <sys/wait.h>
#include <vector>

using namespace nautilus::tracing;

TEST_CASE("SharedTrie basic operations", "[SharedTrie]") {
	auto trie = SharedTrie<uint64_t>::create(100);

	SECTION("initial state") {
		REQUIRE(trie.size() == 1); // just root
		REQUIRE(trie.capacity() == 100);
	}

	SECTION("append single value") {
		auto idx = trie.append(trie.root(), 42);
		REQUIRE(idx != TRIE_NULL);
		REQUIRE(trie[idx].content == 42);
		REQUIRE(trie.size() == 2);
	}

	SECTION("append same value returns same index") {
		auto idx1 = trie.append(trie.root(), 42);
		auto idx2 = trie.append(trie.root(), 42);
		REQUIRE(idx1 == idx2);
		REQUIRE(trie.size() == 2); // no new node created
	}

	SECTION("append different values creates different nodes") {
		auto idx1 = trie.append(trie.root(), 42);
		auto idx2 = trie.append(trie.root(), 43);
		REQUIRE(idx1 != idx2);
		REQUIRE(trie.size() == 3);
	}

	SECTION("build a path") {
		std::vector<uint64_t> path = {100, 200, 300, 400};
		TrieIndex current = trie.root();
		for (auto val : path) {
			current = trie.append(current, val);
		}
		REQUIRE(trie.size() == 5); // root + 4 nodes

		// Traverse again - should get same indices
		TrieIndex current2 = trie.root();
		for (auto val : path) {
			current2 = trie.append(current2, val);
		}
		REQUIRE(current == current2);
		REQUIRE(trie.size() == 5); // no new nodes
	}

	SECTION("diverging paths share common prefix") {
		std::vector<uint64_t> path1 = {1, 2, 3};
		std::vector<uint64_t> path2 = {1, 2, 4};

		TrieIndex end1 = trie.root();
		for (auto val : path1) {
			end1 = trie.append(end1, val);
		}

		TrieIndex end2 = trie.root();
		for (auto val : path2) {
			end2 = trie.append(end2, val);
		}

		REQUIRE(end1 != end2);
		REQUIRE(trie.size() == 5); // root + 1 + 2 + (3,4)
	}

	SECTION("reset clears trie") {
		trie.append(trie.root(), 1);
		trie.append(trie.root(), 2);
		REQUIRE(trie.size() == 3);

		trie.reset();
		REQUIRE(trie.size() == 1);
	}
}

TEST_CASE("SharedTrie insert path", "[SharedTrie]") {
	auto trie = SharedTrie<uint64_t>::create(100);

	SECTION("insert from vector") {
		std::vector<uint64_t> path = {100, 200, 300};
		auto idx = trie.insert(path);
		REQUIRE(trie.size() == 4); // root + 3 nodes
		REQUIRE(trie[idx].content == 300);
	}

	SECTION("insert from initializer list") {
		auto idx = trie.insert({1, 2, 3, 4});
		REQUIRE(trie.size() == 5);
		REQUIRE(trie[idx].content == 4);
	}

	SECTION("insert same path twice returns same index") {
		auto idx1 = trie.insert({10, 20, 30});
		auto idx2 = trie.insert({10, 20, 30});
		REQUIRE(idx1 == idx2);
		REQUIRE(trie.size() == 4); // no new nodes
	}

	SECTION("insert diverging paths") {
		auto idx1 = trie.insert({1, 2, 3});
		auto idx2 = trie.insert({1, 2, 4});
		REQUIRE(idx1 != idx2);
		REQUIRE(trie.size() == 5); // root + 1 + 2 + 3 + 4
	}
}

TEST_CASE("SharedTrie capacity exceeded", "[SharedTrie]") {
	auto trie = SharedTrie<uint64_t>::create(5);

	// Fill up the trie (root counts as 1)
	TrieIndex current = trie.root();
	for (size_t i = 0; i < 4; i++) {
		current = trie.append(current, i);
	}
	REQUIRE(trie.size() == 5);

	// Next append should throw
	REQUIRE_THROWS_AS(trie.append(current, 999), TrieCapacityExceeded);
}

TEST_CASE("SharedTrie with fork", "[SharedTrie][fork]") {
	auto trie = SharedTrie<uint64_t>::create(100);

	// Add something before fork
	TrieIndex parentIdx = trie.append(trie.root(), 1000);
	REQUIRE(trie.size() == 2);

	pid_t pid = fork();
	if (pid == 0) {
		// Child process - trie is still valid (copy of parent's wrapper, same shared memory)
		// Child can see parent's data - should get same index
		auto idx = trie.append(trie.root(), 1000);
		if (idx != parentIdx) {
			_exit(1); // fail if indices don't match
		}
		trie.append(idx, 2000);

		_exit(0);
	} else {
		// Parent
		int status;
		waitpid(pid, &status, 0);
		REQUIRE(WIFEXITED(status));
		REQUIRE(WEXITSTATUS(status) == 0);

		// Parent can see child's modifications
		REQUIRE(trie.size() == 3); // root + 1000 + 2000
	}
	// Destructor only cleans up in parent (creator process)
}

TEST_CASE("SharedTrie move semantics", "[SharedTrie]") {
	auto trie1 = SharedTrie<uint64_t>::create(100);
	trie1.insert({1, 2, 3});
	REQUIRE(trie1.size() == 4);

	// Move construct
	auto trie2 = std::move(trie1);
	REQUIRE(trie2.size() == 4);

	// Move assign
	auto trie3 = SharedTrie<uint64_t>::create(50);
	trie3 = std::move(trie2);
	REQUIRE(trie3.size() == 4);
}
