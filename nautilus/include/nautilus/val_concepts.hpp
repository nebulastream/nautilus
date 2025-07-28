#pragma once
#include "nautilus/config.hpp"
#include <type_traits>
#include <typeinfo>
namespace nautilus {

#ifdef ENABLE_TRACING
#define SHOULD_TRACE() (tracing::inTracer())
#else
#define SHOULD_TRACE() constexpr(false)
#endif

template <typename T>
class val;

template <typename T>
concept convertible_to_integral =
    (std::is_convertible_v<T, int> || std::is_convertible_v<T, char> || std::is_convertible_v<T, long> ||
     std::is_convertible_v<T, short> || std::is_convertible_v<T, unsigned long> ||
     std::is_convertible_v<T, unsigned int> || std::is_convertible_v<T, unsigned short> ||
     std::is_convertible_v<T, long long> || std::is_convertible_v<T, unsigned long long>);

template <typename T>
concept is_fundamental_val = requires {
	typename std::remove_reference_t<T>::basic_type; // Ensure T has a member type 'basic_type'
	requires !std::is_enum_v<typename std::remove_reference_t<T>::basic_type> &&
	             std::is_fundamental_v<typename std::remove_reference_t<T>::basic_type>; // Ensure 'basic_type' is
	                                                                                     // integral
};

template <typename T>
concept convertible_to_fundamental = !std::is_enum_v<T> && !is_fundamental_val<T> &&
                                     (convertible_to_integral<T> || std::is_convertible_v<T, float> ||
                                      std::is_convertible_v<T, double> || std::is_convertible_v<T, bool>);

template <typename T, typename ValT>
concept convertible_to_fundamental_val =
    is_fundamental_val<ValT> && std::is_convertible_v<T, std::remove_cvref_t<ValT>>;

template <typename T>
concept is_arithmetic = std::is_arithmetic_v<T>;

template <typename T>
concept is_fundamental_convertable = std::is_fundamental_v<std::remove_cvref_t<T>> && !std::is_pointer_v<T>;

template <typename T>
concept is_integral_val = requires {
	typename std::remove_reference_t<T>::basic_type; // Ensure T has a member type 'basic_type'
	requires std::is_integral_v<typename std::remove_reference_t<T>::basic_type>; // Ensure 'basic_type' is integral
};

template <typename T, typename ValT>
concept convertible_to_integral_val = is_integral_val<ValT> && std::is_convertible_v<T, std::remove_cvref_t<ValT>>;

template <typename T>
concept is_ptr = std::is_pointer_v<T>;

template <typename T>
concept is_arithmetic_ptr = is_ptr<T> && std::is_arithmetic_v<std::remove_pointer_t<T>>;

template <typename T>
concept is_ptr_ref = is_ptr<std::remove_reference_t<T>> && std::is_reference_v<T>;

template <typename T>
concept is_arithmetic_ref = std::is_arithmetic_v<std::remove_reference_t<T>> && std::is_reference_v<T>;

template <typename T>
concept is_nautilus_ref = is_ptr_ref<T> || is_arithmetic_ref<T>;

template <typename T>
concept is_void_ptr = is_ptr<T> && std::is_void_v<std::remove_pointer_t<T>>;

template <typename T>
concept is_fundamental =
    std::is_fundamental_v<T> && !std::is_reference_v<T> && !std::is_pointer_v<T> && !std::is_same_v<T, bool>;

template <typename T>
concept is_fundamental_ptr = std::is_arithmetic_v<std::remove_pointer_t<T>> && std::is_pointer_v<T>;

template <typename T>
concept is_fundamental_ref = std::is_reference_v<T>;

template <typename T>
concept is_integral = std::is_integral_v<std::remove_cvref_t<T>>;

template <typename T>
concept is_bool = std::is_same_v<T, bool>;

template <typename T>
concept is_bool_ref = std::is_same_v<std::remove_reference_t<T>, bool> && std::is_reference_v<T>;

template <typename T>
concept is_enum = std::is_enum_v<T>;

// checks if the specialization of value is defined
template <typename T>
concept is_val = requires(T a) {
	{ typeid(val<T>) };
};

template <typename T>
struct is_an_val // Default case, no pattern match
    : std::false_type {};

template <typename T>
struct is_an_val<val<T>> // For types matching the pattern A<T>
    : std::true_type {};

template <typename T>
concept is_val_type = is_an_val<T>::value;

// checks if T is a value and it is traceable
template <typename T>
concept is_traceable_value = requires(T a) {
	{ a.state };
};

} // namespace nautilus
