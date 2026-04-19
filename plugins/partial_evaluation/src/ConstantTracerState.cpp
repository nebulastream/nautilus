
// Constant-folding tracer — Stage 1 + Stage 2 implementation.
//
// Previously lived at the top of `nautilus/src/nautilus/tracing/TracingUtil.cpp`.
// Moved here so the trace-time partial-evaluation machinery — Constant
// registry, Stage 2 widening (stratified + iteration-cap), observability
// counters, and runtime flags — is owned by the plugin directory rather
// than diluting the core tracer's source.
//
// This translation unit is only compiled when
// ENABLE_CONSTANT_TRACER is on (the plugin's CMakeLists early-returns
// otherwise). The public declarations for the symbols defined here
// continue to live in `nautilus/tracing/TracingUtil.hpp` — callers see
// a unified interface, but the bodies are here.

#include "nautilus/config.hpp"
#include "nautilus/partial_evaluation/api.hpp"
#include "nautilus/tracing/TracingInterface.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include <cctype>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace nautilus::tracing::pe {

// --- Link-time anchor for PELazyTraceContext.cpp ----------------------------
//
// PELazyTraceContext.cpp contains a file-scope `kRegistrar` static whose
// constructor installs the plugin's trace-override function pointer at
// program start. But that TU has no externally-referenced symbols, so
// when nautilus is linked as a static library into a downstream binary
// the linker dead-strips the TU and the registrar never runs.
//
// ConstantTracerState.cpp (this TU) is always pulled in because val_*.hpp
// fold fast paths reference symbols defined here. We take the address of
// the anchor function below so the linker must resolve the symbol, which
// pulls in PELazyTraceContext.cpp along with its registrar.
void ensurePELazyTraceContextLinked() noexcept;

namespace {
struct ForcePELazyTraceContextLink {
	ForcePELazyTraceContextLink() noexcept {
		ensurePELazyTraceContextLinked();
	}
};
[[maybe_unused]] const ForcePELazyTraceContextLink kForceLink {};
} // namespace

// --- Constant registry ------------------------------------------------------

// Thread-local registry of currently-Constant val<T> state holders. An
// intrusive doubly-linked list with a sentinel head — register/unregister
// are O(1) and allocation-free; iteration is a linear walk over the list.
//
// Insertion order is the registration order, which is deterministic as long
// as val<T> construction order is deterministic across trace iterations
// (it is — same function, same code, same object lifetimes on the same
// stack). That's the property materializeAllConstants() depends on for
// follow-mode replay to reproduce the exact CONST sequence iter 1
// recorded; a hash-bucketed map would have lost that.
static thread_local ConstantRegistryNode constantRegistryHead;

namespace {
// Lazily initialize the sentinel to point at itself (empty list). Called
// on the first register per trace; cheap and branch-predictable.
inline void ensureRegistryInit() {
	if (constantRegistryHead.next == nullptr) {
		constantRegistryHead.prev = &constantRegistryHead;
		constantRegistryHead.next = &constantRegistryHead;
	}
}

inline bool registryEmpty() {
	return constantRegistryHead.next == nullptr || constantRegistryHead.next == &constantRegistryHead;
}
} // namespace

// --- Stage 2 widening state -------------------------------------------------

// constantAssignmentCount — incremented on every Constant-to-Constant
// assignment; flushed (set back to 0) whenever materializeAllConstants()
// runs. Crossing the threshold forces a flush. Blunt backstop: fires
// after N rounds even when no back-edge is actually detected (e.g. a
// straight-line program that happens to do many Constant-to-Constant
// assignments).
//
// valueHashByTag — Stage 2 stratified detection. Keys Tag* (a stable
// pointer-identity identifier of a program point, shared across trace
// iterations by Nautilus's TagRecorder trie), values the registry-wide
// value hash at the most recent visit. A second visit to the same Tag
// with a different value hash means we're re-entering a program point
// with changed Constant values — the back-edge of a loop whose body
// folds at C++ level. Trigger: immediate materializeAllConstants().
static thread_local uint64_t constantAssignmentCount = 0;

struct TagStratificationState {
	uint64_t hash;
	uint64_t divergences;
};
static thread_local std::unordered_map<const void*, TagStratificationState> valueHashByTag;

// --- Observability counters -------------------------------------------------

// Reset at the start of each trace (see resetCountersForNewTrace below,
// called from setActiveTracer) and readable via the public getters until
// the next trace begins, so the compiler can snapshot them into
// CompilationStatistics between trace-end and compilation-start.
static thread_local uint64_t ctrFoldsElided = 0;
static thread_local uint64_t ctrFlushes = 0;
static thread_local uint64_t ctrWideningsCap = 0;
static thread_local uint64_t ctrWideningsStrat = 0;

// --- Env-var-driven runtime flags -------------------------------------------

namespace {
// Default widening threshold — picked to be large enough to keep small
// legitimate constant chains zero-overhead, while still catching the
// 10^5..10^6 trip-count pathology well before MaxTracingIterations.
constexpr uint64_t DEFAULT_WIDEN_AT = 1024;

// Parse an unsigned integer env var, returning the default if unset or
// malformed.
[[nodiscard]] uint64_t parseUintEnv(const char* name, uint64_t fallback) {
	const char* raw = std::getenv(name); // NOLINT(concurrency-mt-unsafe) init-only use
	if (raw == nullptr || *raw == '\0') {
		return fallback;
	}
	char* end = nullptr;
	errno = 0;
	unsigned long long parsed = std::strtoull(raw, &end, 10);
	if (errno != 0 || end == raw) {
		return fallback;
	}
	return static_cast<uint64_t>(parsed);
}

} // namespace

// Cached per-thread flags. thread_local initialization runs on first access.
//
// `constantTracerEnabled` defaults to false and is flipped on at
// `PELazyTraceContext::onActivate()` / off at `onDeactivate()` so the
// flag tracks the active trace mode automatically. The NAUTILUS_
// CONSTANT_TRACER_ENABLED env var is retired — mode selection is the
// single source of truth and A/B runs switch `engine.traceMode` rather
// than toggling a separate flag.
static thread_local bool constantTracerEnabled = false;
static thread_local uint64_t constantWideningThreshold =
    parseUintEnv("NAUTILUS_CONSTANT_TRACER_WIDEN_AT", DEFAULT_WIDEN_AT);
// Default 0 = widen on first divergent revisit. Set >0 to allow N rounds
// of full trace-time unrolling per program point, or UINT64_MAX to disable
// stratification entirely and fall back to the blunt iteration cap.
static thread_local uint64_t constantUnrollLimit = parseUintEnv("NAUTILUS_CONSTANT_TRACER_UNROLL_LIMIT", 0);

bool isConstantTracerEnabled() noexcept {
	return constantTracerEnabled;
}

void setConstantTracerEnabled(bool enabled) noexcept {
	constantTracerEnabled = enabled;
}

uint64_t getConstantWideningThreshold() noexcept {
	return constantWideningThreshold;
}

void setConstantWideningThreshold(uint64_t threshold) noexcept {
	constantWideningThreshold = threshold;
}

uint64_t getConstantUnrollLimit() noexcept {
	return constantUnrollLimit;
}

void setConstantUnrollLimit(uint64_t limit) noexcept {
	constantUnrollLimit = limit;
}

uint64_t getConstantTracerFoldsElided() noexcept {
	return ctrFoldsElided;
}
uint64_t getConstantTracerFlushes() noexcept {
	return ctrFlushes;
}
uint64_t getConstantTracerWideningsCap() noexcept {
	return ctrWideningsCap;
}
uint64_t getConstantTracerWideningsStrat() noexcept {
	return ctrWideningsStrat;
}

void noteConstantFoldElided() noexcept {
	if (tracing::getActiveTracer() != nullptr) {
		++ctrFoldsElided;
	}
}

void clearConstantRegistry() noexcept {
	// Walks the intrusive list, unlinks every node. Doesn't call
	// materialize_fn on any of them — the caller has asserted (by
	// invoking this) that the registry's contents are stale/abandoned.
	ensureRegistryInit();
	while (constantRegistryHead.next != &constantRegistryHead) {
		auto* n = constantRegistryHead.next;
		n->prev->next = n->next;
		n->next->prev = n->prev;
		n->prev = nullptr;
		n->next = nullptr;
		n->materialize_fn = nullptr;
	}
	constantAssignmentCount = 0;
	valueHashByTag.clear();
}

void resetCountersForNewTrace() noexcept {
	// Called from core's setActiveTracer(non-null). Zeroes the observability
	// counters so a post-trace getConstantTracer*Count reads exactly that
	// one trace's events.
	ctrFoldsElided = 0;
	ctrFlushes = 0;
	ctrWideningsCap = 0;
	ctrWideningsStrat = 0;
}

void registerConstant(ConstantRegistryNode* node, ConstantMaterializeFn materialize_fn) {
	// Gate on the runtime flag even when built with ENABLE_CONSTANT_TRACER.
	// If the runtime flag is off, we leave the registry empty so the compile
	// time fold fast paths (which also check the flag) never get to call this.
	if (tracing::getActiveTracer() == nullptr || !constantTracerEnabled) {
		return;
	}
	ensureRegistryInit();
	node->materialize_fn = materialize_fn;
	// Insert at head->prev (tail) to preserve registration order when we
	// walk forward from head->next; this matches what std::map did for
	// address-sorted keys only coincidentally — intrusive order is simply
	// construction order, which is exactly what follow-mode replay needs.
	node->prev = constantRegistryHead.prev;
	node->next = &constantRegistryHead;
	constantRegistryHead.prev->next = node;
	constantRegistryHead.prev = node;
}

void unregisterConstant(ConstantRegistryNode* node) {
	// O(1) unlink. Safe on an un-registered node (next == nullptr).
	if (node->next == nullptr) {
		return;
	}
	node->prev->next = node->next;
	node->next->prev = node->prev;
	node->prev = nullptr;
	node->next = nullptr;
	node->materialize_fn = nullptr;
}

void materializeAllConstants() {
	// Flushing counts as "no unbounded trace-time unroll in progress".
	constantAssignmentCount = 0;
	// Stale tag->value_hash entries — every Constant we just materialized
	// no longer exists, and any lingering tag keys would cause false-
	// negative divergence at a different, unrelated program point.
	valueHashByTag.clear();
	if (tracing::getActiveTracer() == nullptr || registryEmpty()) {
		return;
	}
	// Count this as a real flush only when there was actually work to do.
	// Empty-registry calls aren't interesting for the observability metric.
	++ctrFlushes;
	// Snapshot holder pointers + thunks in registration order. Each
	// materialize() call unregisters its holder, which mutates the list; a
	// direct walk would break. The snapshot is tiny (O(n) pointers) and
	// linearly scanned for cache-friendliness.
	std::vector<std::pair<ConstantRegistryNode*, ConstantMaterializeFn>> snapshot;
	for (auto* cur = constantRegistryHead.next; cur != &constantRegistryHead; cur = cur->next) {
		snapshot.emplace_back(cur, cur->materialize_fn);
	}
	for (auto& [node, fn] : snapshot) {
		fn(node);
	}
}

namespace {
/// O(n) sum over the live registry's per-node value_hash fields. Sequential
/// combine (rather than XOR) so permutations hash differently — but
/// insertion order is stable across trace iterations (same stack, same C++
/// object lifetimes) so the sum is reproducible.
[[nodiscard]] uint64_t computeRegistryValueHash() noexcept {
	uint64_t h = 0xcbf29ce484222325ULL; // FNV-1a-ish seed; anything not-zero works.
	for (auto* cur = constantRegistryHead.next; cur != nullptr && cur != &constantRegistryHead; cur = cur->next) {
		h = h * 1099511628211ULL + cur->value_hash;
	}
	return h;
}
} // namespace

void noteConstantAssignment() {
	// Fast path: not tracing, nothing to do.
	tracing::TracingInterface* tracer = tracing::getActiveTracer();
	if (tracer == nullptr) {
		return;
	}

	// Stage 2 stratified divergence detection. Identify the current program
	// point by its call-stack Tag (TagRecorder interns Tags per source line,
	// so Tag* compares equal across trace iterations for the same source
	// location). Compare the registry-wide value hash against what we saw
	// the last time we were here:
	//   - first visit: record and move on
	//   - same hash: invariant state, ignore
	//   - different hash: back-edge with changed Constant values — a loop
	//     body that folds at C++ level. Widen immediately; subsequent runs
	//     go through the real tracer and the Snapshot-match machinery takes
	//     over as usual.
	// This catches runaway trace-time unrolling in 2 loop iterations
	// instead of waiting for the blunt iteration-cap backstop.
	const void* tag = tracer->currentTag();
	if (tag != nullptr) {
		uint64_t currentHash = computeRegistryValueHash();
		auto [it, inserted] = valueHashByTag.try_emplace(tag, TagStratificationState {currentHash, 0});
		if (!inserted) {
			if (it->second.hash != currentHash) {
				// Divergent revisit. If the per-tag divergence count is
				// still under the unroll limit, record the new hash and
				// keep unrolling; otherwise widen.
				it->second.hash = currentHash;
				if (it->second.divergences >= constantUnrollLimit) {
					// materializeAllConstants() clears the map itself, so
					// we're done after it returns.
					++ctrWideningsStrat;
					materializeAllConstants();
					return;
				}
				++it->second.divergences;
				// Fall through to the blunt cap so runaway traces can't
				// escape it even with a high unroll limit.
			} else {
				// Same tag, same hash: true invariant; don't bump the blunt
				// iteration-cap counter either — otherwise stable-valued
				// re-visits would tick up and force a spurious flush.
				return;
			}
		}
	}

	// Blunt iteration-cap backstop. Fires when stratification doesn't catch
	// the case (e.g. straight-line code with lots of distinct Constant
	// assignments, or when currentTag() returns null).
	// Threshold=0 disables the backstop entirely — useful for benchmarking
	// the unrolling cost in isolation.
	if (constantWideningThreshold == 0) {
		return;
	}
	if (++constantAssignmentCount >= constantWideningThreshold) {
		++ctrWideningsCap;
		materializeAllConstants();
	}
}

} // namespace nautilus::tracing::pe
