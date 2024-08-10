#pragma once
#include "nautilus/config.hpp"
#include <type_traits>
namespace nautilus {

#ifdef ENABLE_TRACING
#define SHOULD_TRACE() (tracing::inTracer())
#else
#define SHOULD_TRACE() constexpr(false)
#endif

template <typename T>
concept convertible_to_fundamental =
    (std::is_convertible_v<T, int> || std::is_convertible_v<T, double> || std::is_convertible_v<T, char> ||
     std::is_convertible_v<T, bool> || std::is_convertible_v<T, float> || std::is_convertible_v<T, long> ||
     std::is_convertible_v<T, short> || std::is_convertible_v<T, unsigned long> ||
     std::is_convertible_v<T, unsigned int> || std::is_convertible_v<T, unsigned short> ||
     std::is_convertible_v<T, long long> || std::is_convertible_v<T, unsigned long long>);

template <typename T>
concept is_arithmetic = std::is_arithmetic_v<T>;

template <class T>
concept is_ptr = std::is_pointer_v<T>;

template <class T>
concept is_arithmetic_ptr = is_ptr<T> && std::is_arithmetic_v<std::remove_pointer_t<T>>;

template <class T>
concept is_arithmetic_ref = std::is_arithmetic_v<std::remove_reference_t<T>> && std::is_reference_v<T>;

template <class T>
concept is_void_ptr = is_ptr<T> && std::is_void_v<std::remove_pointer_t<T>>;

template <class T>
concept is_fundamental =
    std::is_fundamental_v<T> && !std::is_reference_v<T> && !std::is_pointer_v<T> && !std::is_same_v<T, bool>;

template <class T>
concept is_fundamental_ptr = std::is_arithmetic_v<std::remove_pointer_t<T>> && std::is_pointer_v<T>;

template <class T>
concept is_member_ptr = !std::is_fundamental_v<std::remove_pointer_t<T>> && std::is_pointer_v<T>;

template <class T>
concept is_fundamental_ref = std::is_reference_v<T>;

template <class T>
concept is_integral = std::is_integral_v<T>;

template <class T>
concept is_bool = std::is_same_v<T, bool>;

template <class T>
concept is_bool_ref = std::is_same_v<std::remove_reference<T>, bool> && std::is_reference_v<T>;

template <class T>
concept is_enum = std::is_enum_v<T>;

template <typename T>
class val;

// checks if the specialization of value is defined
template <typename T>
concept is_val = requires(T a) {
	{ typeid(val<T>) };
};

template <typename T>
concept is_val_type = requires(val<T> a) {
	{ typeid(T) };
};

template <typename T, typename = void>
struct has_state_member : std::false_type {};

template <typename T>
struct has_state_member<T, std::void_t<decltype(std::declval<T>().state)>> : std::true_type {};

template <typename T>
constexpr bool has_state_member_v = has_state_member<T>::value;

// checks if T is a value and it is traceable
template <typename T>
concept is_traceable_value = requires(T a) {
	{ a.state };
};

} // namespace nautilus