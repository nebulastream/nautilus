
/**
 * @file val_std.hpp
 * @brief Nautilus val<T> specialization for C++ class (struct) types.
 *
 * This header provides a `val<T>` partial template specialization for any C++ class/struct type
 * that is not itself a pointer. It enables traceable, stack-allocated value semantics for
 * user-defined aggregate types inside Nautilus-compiled functions.
 *
 * ## How it works
 *
 * A `val<MyStruct>` stores its data via an internal `val<MyStruct*>` that points to
 * engine-managed memory:
 *   - **Tracing mode**: the allocation is recorded as an IR `AllocaOperation`, which the
 *     MLIR/LLVM backend lowers to an `alloca` (stack slot).
 *   - **Interpreted mode**: memory comes from the thread-local `AllocationContext` arena that
 *     the engine sets up before each function call; it is freed automatically when the
 *     arena is destroyed at the call boundary.
 *
 * Construction, destruction and copying are all forwarded through `invoke()` so that
 * the tracing engine can record them as proper IR runtime-call operations.
 *
 * ## Supported operations
 *
 * | Operation                  | Supported | Notes                                              |
 * |----------------------------|-----------|----------------------------------------------------|
 * | Default construction       | Yes       | Non-trivial ctor forwarded through `invoke()`      |
 * | Parameterised construction | Yes       | `val<T>(val<A>, val<B>, …)` — args unwrapped via `unwrap_val_t` |
 * | Copy construction          | Yes       | `memcpy` for trivially-copyable; otherwise `invoke`|
 * | Copy assignment            | Yes       | same as copy construction                          |
 * | Move construction/assignment | No      | Not provided; use copy                             |
 * | Destruction                | Yes       | Non-trivial dtor forwarded through `invoke()`      |
 * | Field read  (`get`)        | Yes       | Returns `val<F&>` for arithmetic/pointer fields    |
 * | Field write (`set`)        | Yes       | Accepts `val<F>` or plain `F`                      |
 * | Arithmetic / comparison operators | No | Access fields then operate on their `val<>`s      |
 * | Method calls               | Via `invoke()` | Wrap member functions with a free function    |
 *
 * ## Rules and constraints
 *
 * 1. **Class types only**: The constraint is `std::is_class_v<T> && !std::is_pointer_v<T>`.
 *    This covers `struct` and `class` types. It does *not* cover:
 *    - Fundamental types (int, float, …) → use `val_arith.hpp`
 *    - Pointer types                      → use `val_ptr.hpp`
 *    - Enum types                         → use `val_enum.hpp`
 *    - Union types are technically class types but their aliasing rules make them unsafe here.
 *
 * 2. **Construction**: `val<T>` supports default construction and parameterised construction
 *    via `val<T>(val<A> a, val<B> b, …)`. Each argument's raw type is deduced through
 *    `details::unwrap_val_t` and a concrete `construct_with<RawArgs…>` instantiation is
 *    forwarded to `invoke()`, so the constructor call is recorded in the IR trace.
 *    Plain (non-val) argument types are also accepted and passed through directly.
 *    There is no constructor that wraps an existing `T` value; to work with an externally
 *    owned object, pass a pointer: `val<T*>`.
 *
 * 3. **Field access via member pointers**: `get()`/`set()` accept a pointer-to-member (`F T::*`).
 *    The field offset is computed once using `field_offset()` (pointer arithmetic on a local
 *    buffer). This works correctly for:
 *    - Direct data members of arithmetic or pointer types.
 *    Limitations:
 *    - Nested class fields are not yet supported (returning `val<NestedStruct>` from `get()`).
 *    - Base-class members require an explicit pointer cast first (`val<Base*>`).
 *    - Bitfield members cannot form a member pointer (C++ restriction).
 *    - Virtual-base or multiple-inheritance layouts may produce incorrect offsets.
 *
 * 4. **No arithmetic/comparison operators on `val<T>`**: The class-type specialization only
 *    provides structural access (get/set). Operate on the retrieved `val<Field>` values instead.
 *
 * 5. **Engine-managed lifetime**: In non-tracing mode, `AllocationContext::get()` must return a
 *    non-null pointer. This is guaranteed when the function is called through the engine
 *    (`Engine::Compile`/`CompiledFunction::operator()`). Constructing a `val<ClassType>`
 *    outside an engine call will trigger an assertion failure.
 *
 * 6. **No virtual dispatch inside traced code**: Virtual method calls are not tracked by the
 *    tracer. Use `invoke()` with a non-virtual free function or lambda instead.
 *
 * ## Usage example
 *
 * ```cpp
 * #include <nautilus/val_std.hpp>
 *
 * struct Point {
 *     int32_t x, y;
 *     Point() : x(0), y(0) {}
 *     Point(int32_t x, int32_t y) : x(x), y(y) {}
 * };
 *
 * // Default construction + field set/get:
 * val<int32_t> sumPoint() {
 *     val<Point> p;                                    // default-constructs Point()
 *     p.set(&Point::x, 3);                             // traces a constant store
 *     p.set(&Point::y, 4);
 *     return p.get(&Point::x) + p.get(&Point::y);      // traced loads + add
 * }
 *
 * // Parameterised construction from traced values:
 * val<int32_t> sumPointArgs(val<int32_t> px, val<int32_t> py) {
 *     val<Point> p(px, py);                            // calls Point(int32_t, int32_t) via invoke
 *     return p.get(&Point::x) + p.get(&Point::y);
 * }
 *
 * // Passing a pointer to an external struct:
 * val<int32_t> getX(val<Point*> ptr) {
 *     return ptr.get(&Point::x);   // get() is also available on val<T*>
 * }
 * ```
 */

#pragma once

#include "nautilus/function.hpp"
#include "nautilus/std/cstring.h"
#include "nautilus/val_concepts.hpp"
#include <cstring>
#include <type_traits>

namespace nautilus {

namespace details {

/// Strips val<T> wrappers to their inner raw type.
/// val<int32_t>  → int32_t
/// val<int32_t*> → int32_t*
/// int32_t       → int32_t  (passthrough for plain types)
template <typename T>
struct unwrap_val {
	using type = T;
};
template <typename T>
struct unwrap_val<val<T>> {
	using type = T;
};
template <typename T>
using unwrap_val_t = typename unwrap_val<std::remove_cvref_t<T>>::type;

template <typename ValueType>
val<ValueType*> nautilus_alloca() {
#ifdef ENABLE_TRACING
	if (tracing::inTracer()) {
		auto valueRef = tracing::traceAlloca(sizeof(ValueType));
		return val<ValueType*>(valueRef);
	}
#endif
	ValueType* ptr = static_cast<ValueType*>(::operator new(sizeof(ValueType)));
	return val<ValueType*>(ptr);
}
} // namespace details

/**
 * val<T> specialization for C++ class/struct types (non-pointer).
 *
 * See the file-level documentation above for the full usage guide and limitations.
 */
template <typename ValueType>
    requires(std::is_class_v<ValueType> && !std::is_pointer_v<ValueType>)
class val<ValueType> {
private:
	// All data lives behind a traced pointer so that loads, stores and the
	// alloca itself appear as first-class operations in the IR.
	val<ValueType*> value_ptr;

	static void construct(ValueType* ptr) {
		new (ptr) ValueType();
	}

	static void destruct(ValueType* ptr) {
		ptr->~ValueType();
	}

	static void copy_construct(ValueType* dst, ValueType* src) {
		new (dst) ValueType(*src);
	}

	static void copy_assign(ValueType* dst, ValueType* src) {
		*dst = *src;
	}

	// Generic placement-new helper for parameterised constructors.
	// A concrete instantiation (e.g. construct_with<int32_t, float>) is a plain function
	// pointer that invoke() can trace as a regular runtime call.
	template <typename... RawArgs>
	static void construct_with(ValueType* ptr, RawArgs... args) {
		new (ptr) ValueType(args...);
	}

public:
	// Default-constructs the object on the traced stack.
	// For trivially-default-constructible types the ctor call is elided.
	val() : value_ptr(details::nautilus_alloca<ValueType>()) {
		if constexpr (!std::is_trivially_default_constructible_v<ValueType>) {
			invoke(val<ValueType>::construct, value_ptr);
		}
	}

	// Copy-constructs from another val<ValueType>.
	// Trivially-copyable types use a traced memcpy; others use copy_construct via invoke().
	val(const val<ValueType>& other) : value_ptr(details::nautilus_alloca<ValueType>()) {
		if constexpr (std::is_trivially_copyable_v<ValueType>) {
			nautilus::memcpy(value_ptr, other.value_ptr, sizeof(ValueType));
		} else {
			invoke(copy_construct, value_ptr, other.value_ptr);
		}
	}

	// Constructs the object from one or more traced (val<T>) or plain arguments.
	// Each argument's raw type is deduced via unwrap_val_t, which produces the concrete
	// construct_with<RawArgs...> instantiation passed to invoke().
	// The non-template copy constructor above always wins for same-type copies, so
	// this template never conflicts with it.
	template <typename... ValArgs>
	    requires(sizeof...(ValArgs) > 0)
	val(ValArgs&&... args) : value_ptr(details::nautilus_alloca<ValueType>()) {
		invoke(construct_with<details::unwrap_val_t<ValArgs>...>, value_ptr, std::forward<ValArgs>(args)...);
	}

	// Copy-assigns from another val<ValueType>.
	// Trivially-copyable types use a traced memcpy; others use copy_assign via invoke().
	val<ValueType>& operator=(const val<ValueType>& other) {
		if constexpr (std::is_trivially_copyable_v<ValueType>) {
			nautilus::memcpy(value_ptr, other.value_ptr, sizeof(ValueType));
		} else {
			invoke(copy_assign, value_ptr, other.value_ptr);
		}
		return *this;
	}

	/**
	 * Read a direct data member.
	 *
	 * @tparam F   Field type (must be arithmetic or pointer — class-typed fields are not yet supported).
	 * @param  pm  Pointer-to-member (e.g. `&MyStruct::field`).
	 * @return     A `val<F&>` reference that can be converted to `val<F>` for a load
	 *             or used as an lvalue for a store.
	 *
	 * Limitation: `pm` must designate a *direct* member. Base-class members require
	 * casting the internal pointer to `val<Base*>` first.
	 */
	template <typename F, typename T = ValueType>
	    requires std::is_class_v<T>
	auto get(F T::*pm) {
		return value_ptr.get(pm);
	}

	/**
	 * Write a direct data member from a traced value.
	 *
	 * @param pm     Pointer-to-member.
	 * @param value  The traced value to store.
	 */
	template <typename F, typename T = ValueType>
	    requires std::is_class_v<T>
	void set(F T::*pm, val<F> value) {
		return value_ptr.set(pm, value);
	}

	/**
	 * Write a direct data member from a plain (compile-time constant) value.
	 *
	 * The constant is wrapped in a `val<F>` constant operation before being stored,
	 * allowing the optimizer to propagate it.
	 *
	 * @param pm     Pointer-to-member.
	 * @param value  The constant value to store.
	 */
	template <typename F, typename T = ValueType>
	    requires std::is_class_v<T>
	void set(F T::*pm, F value) {
		return value_ptr.set(pm, value);
	}

	/**
	 * Returns a val<ValueType*> pointing to the engine-managed storage of this object.
	 *
	 * This is the primary mechanism for passing a stack-allocated val<Struct> as an
	 * output parameter to an invoke'd runtime function. Because runtime functions
	 * cannot return val<T> directly, the idiomatic pattern is:
	 *
	 *   val<Result> result;
	 *   invoke(myRuntimeFunc, &result, arg1, arg2);   // fills result via pointer
	 *   return result.get(&Result::field);
	 *
	 * In tracing mode the returned val<ValueType*> carries the SSA reference of
	 * the alloca so that all subsequent loads and stores through the pointer are
	 * correctly recorded in the IR.
	 */
	val<ValueType*> operator&() {
		return value_ptr;
	}

	// Destroys the object. For trivially-destructible types the dtor call is elided.
	~val() {
		if constexpr (!std::is_trivially_destructible_v<ValueType>) {
			invoke(destruct, value_ptr);
		}
		// in interpreter mode the value is allocated on heap, so we remove the allocation here
#ifdef ENABLE_TRACING
		if (!tracing::inTracer()) {
			::operator delete(value_ptr.value);
		}
#else
		::operator delete(value_ptr.value);
#endif
	}
};
} // namespace nautilus
