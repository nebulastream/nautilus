

#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/nautilus_function.hpp>
namespace nautilus::engine {


    val<int> add_indirect(val<int> a, val<int> b) {
        return a + b;
    }

    static auto nautilusAdd = NautilusFunction{"add", add_indirect};

    val<int> nautilusFunctionExample(val<int> a, val<int> b) {
        auto result = nautilusAdd(a, b);
        return result;
    }

}