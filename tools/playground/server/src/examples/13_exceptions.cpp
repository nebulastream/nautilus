// title: Exception handling & landing pads
// description: A call that is not noexcept becomes CALL_WITH_EXCEPTION_HANDLING in the trace, and the IR grows an exception_region whose pad runs the live destructors.
#include <nautilus/Engine.hpp>
#include <nautilus/function.hpp>
#include <nautilus/val.hpp>
#include <nautilus/val_std.hpp>

#include <stdexcept>

using namespace nautilus;

// A non-trivial destructor makes a val<Guard> live cleanup state: every
// potentially-throwing call below it records the destructor to run while
// unwinding.
struct Guard {
    int32_t value = 0;
    ~Guard() noexcept {
    }
};

// noexcept, so this stays on the direct path: a plain CALL, no pad.
void store(Guard* guard, int32_t value) noexcept {
    guard->value = value;
}

// NOT noexcept: traced as CALL_WITH_EXCEPTION_HANDLING with the live
// destructor in its `unwind[...]` list. After ExceptionRegionPreparationPass
// the call site carries `-> pad_0`, and the pad itself shows up under
// `exception_region:` at the end of the function.
int32_t checkedDivide(int32_t a, int32_t b) {
    if (b == 0) {
        throw std::domain_error("division by zero");
    }
    return a / b;
}

void playground_register(engine::NautilusModule& m) {
    m.registerFunction<val<int32_t>(val<int32_t>, val<int32_t>)>(
        "divideWithCleanup", [](val<int32_t> a, val<int32_t> b) {
            val<Guard> guard;
            invoke(store, &guard, a);
            val<int32_t> quotient = invoke(checkedDivide, a, b);
            return quotient + guard.get(&Guard::value);
        });
}
