#pragma once

#include <any>
#include <cstdint>
#include <cstring>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

namespace nautilus::compiler {

class CompilationStatistics;

/**
 * @brief Represents an executable object, which enables the invocation of dynamically defined methods.
 */
class Executable {
public:
	/**
	 * @brief A general wrapper for the invocation of an executable function.
	 */
	class GenericInvocable {
	public:
		/**
		 * @brief Invoke executable function with a set of arguments.
		 * @param args set of arguments
		 * @return result
		 */
		virtual std::any invokeGeneric(const std::vector<std::any>& args) = 0;

		/**
		 * @brief Optional allocation-free fast path: arguments and result as
		 * raw 64-bit slots (integers zero-extended, bools 0/1, floats and
		 * pointers bit-exact). Returns false when unsupported, in which case
		 * the caller falls back to invokeGeneric.
		 */
		virtual bool invokeRaw(const uint64_t* /*args*/, size_t /*argCount*/, uint64_t* /*result*/) {
			return false;
		}

		void setGeneratedFiles(const std::map<std::string, std::string>& generatedFiles);

		virtual ~GenericInvocable() = default;

	private:
		std::map<std::string, std::string> generatedFiles;
	};

	/**
	 * @brief A typed invocable wrapper that is callable as a function and forwards arguments a concrete implementation.
	 * @tparam R return type
	 * @tparam Args agument types
	 */
	template <typename R, typename... Args>
	class Invocable {
		using FunctionType = R(Args...);
		std::variant<FunctionType*, std::unique_ptr<GenericInvocable>> func;

	public:
		explicit Invocable(void* fptr) : func(reinterpret_cast<FunctionType*>(fptr)) {
		}

		explicit Invocable(std::unique_ptr<GenericInvocable> generic) : func(std::move(generic)) {
		}

		template <typename T>
		    requires(std::is_fundamental_v<T> || std::is_fundamental_v<std::remove_cvref_t<T>>)
		std::any getGenericArg(T&& val) {
			return val;
		}

		template <typename T>
		    requires std::is_pointer_v<std::remove_cvref_t<T>>
		std::any getGenericArg(T&& val) {
			return std::any((void*) val);
		}

		template <typename T>
		    requires std::is_enum_v<std::remove_cvref_t<T>>
		std::any getGenericArg(T&& val) {
			return std::any((std::underlying_type_t<std::remove_cvref_t<T>>) val);
		}

		/// Convert a boxed arithmetic result whose exact C++ type differs from
		/// R. Backends box per nautilus IR type, but distinct C++ types share
		/// one IR type (char vs int8_t; long vs long long for i64 depending on
		/// the platform), so an exact any_cast cannot be relied upon.
		template <typename T>
		static T convertBoxedArithmetic(const std::any& res) {
			if constexpr (std::is_integral_v<T>) {
				if (const auto* v = std::any_cast<bool>(&res)) {
					return static_cast<T>(*v);
				}
				if (const auto* v = std::any_cast<char>(&res)) {
					return static_cast<T>(*v);
				}
				if (const auto* v = std::any_cast<signed char>(&res)) {
					return static_cast<T>(*v);
				}
				if (const auto* v = std::any_cast<unsigned char>(&res)) {
					return static_cast<T>(*v);
				}
				if (const auto* v = std::any_cast<short>(&res)) {
					return static_cast<T>(*v);
				}
				if (const auto* v = std::any_cast<unsigned short>(&res)) {
					return static_cast<T>(*v);
				}
				if (const auto* v = std::any_cast<int>(&res)) {
					return static_cast<T>(*v);
				}
				if (const auto* v = std::any_cast<unsigned int>(&res)) {
					return static_cast<T>(*v);
				}
				if (const auto* v = std::any_cast<long>(&res)) {
					return static_cast<T>(*v);
				}
				if (const auto* v = std::any_cast<unsigned long>(&res)) {
					return static_cast<T>(*v);
				}
				if (const auto* v = std::any_cast<long long>(&res)) {
					return static_cast<T>(*v);
				}
				if (const auto* v = std::any_cast<unsigned long long>(&res)) {
					return static_cast<T>(*v);
				}
			} else {
				if (const auto* v = std::any_cast<float>(&res)) {
					return static_cast<T>(*v);
				}
				if (const auto* v = std::any_cast<double>(&res)) {
					return static_cast<T>(*v);
				}
			}
			throw std::bad_any_cast();
		}

		/// Whether T can travel through the raw 64-bit-slot fast path.
		template <typename T>
		static constexpr bool isRawSlotCompatible() {
			using D = std::remove_cvref_t<T>;
			return std::is_void_v<D> || std::is_enum_v<D> || std::is_pointer_v<D> || std::is_integral_v<D> ||
			       std::is_same_v<D, float> || std::is_same_v<D, double>;
		}

		/// Normalize a value into a raw 64-bit slot (integers zero-extended,
		/// bools 0/1, floats and pointers bit-exact) — the register-slot
		/// convention shared by interpreting backends.
		template <typename T>
		static uint64_t toRawSlot(T value) {
			using D = std::remove_cvref_t<T>;
			if constexpr (std::is_enum_v<D>) {
				return toRawSlot(static_cast<std::underlying_type_t<D>>(value));
			} else if constexpr (std::is_pointer_v<D>) {
				return reinterpret_cast<uint64_t>(value);
			} else if constexpr (std::is_same_v<D, bool>) {
				return value ? 1 : 0;
			} else if constexpr (std::is_integral_v<D>) {
				return static_cast<uint64_t>(static_cast<std::make_unsigned_t<D>>(value));
			} else if constexpr (std::is_same_v<D, float>) {
				uint32_t bits = 0;
				std::memcpy(&bits, &value, sizeof(bits));
				return bits;
			} else {
				static_assert(std::is_same_v<D, double>);
				uint64_t bits = 0;
				std::memcpy(&bits, &value, sizeof(bits));
				return bits;
			}
		}

		/// Read a raw 64-bit result slot as T (the low bits are authoritative
		/// for narrow types).
		template <typename T>
		static T fromRawSlot(uint64_t raw) {
			if constexpr (std::is_enum_v<T>) {
				return static_cast<T>(fromRawSlot<std::underlying_type_t<T>>(raw));
			} else if constexpr (std::is_pointer_v<T>) {
				return reinterpret_cast<T>(raw);
			} else if constexpr (std::is_same_v<T, bool>) {
				return raw != 0;
			} else if constexpr (std::is_integral_v<T>) {
				return static_cast<T>(raw);
			} else if constexpr (std::is_same_v<T, float>) {
				float value;
				const auto bits = static_cast<uint32_t>(raw);
				std::memcpy(&value, &bits, sizeof(value));
				return value;
			} else {
				static_assert(std::is_same_v<T, double>);
				double value;
				std::memcpy(&value, &raw, sizeof(value));
				return value;
			}
		}

		/// Unbox a generic invocation result as R. Handles enums (boxed as
		/// their underlying type), pointers (boxed as void*), and arithmetic
		/// results boxed as a same-width alias of R.
		template <typename T>
		static T castGenericResult(const std::any& res) {
			if constexpr (std::is_enum_v<T>) {
				return static_cast<T>(castGenericResult<std::underlying_type_t<T>>(res));
			} else if constexpr (std::is_pointer_v<T>) {
				return (T) std::any_cast<void*>(res);
			} else if constexpr (std::is_arithmetic_v<T>) {
				if (const T* exact = std::any_cast<T>(&res)) {
					return *exact;
				}
				return convertBoxedArithmetic<T>(res);
			} else {
				return std::any_cast<T>(res);
			}
		}
		/**
		 * @brief Invoke the function with a set of arguments
		 * @param arguments
		 * @return returns the result of the function if any
		 */
		R operator()(Args... arguments) {
			if (std::holds_alternative<FunctionType*>(func)) {
				auto fptr = std::get<FunctionType*>(func);
				if constexpr (!std::is_void_v<R>) {
					return fptr(std::forward<Args>(arguments)...);
				} else {
					fptr(std::forward<Args>(arguments)...);
				}
			} else {
				auto& genericFunction = std::get<std::unique_ptr<GenericInvocable>>(func);
				// Allocation-free fast path: pass arguments as raw 64-bit
				// slots on the stack. Backends that don't implement invokeRaw
				// return false and take the boxed route below.
				if constexpr (isRawSlotCompatible<R>() && (isRawSlotCompatible<Args>() && ...)) {
					const uint64_t rawArgs[sizeof...(Args) + 1] = {toRawSlot(arguments)...};
					uint64_t rawResult = 0;
					if (genericFunction->invokeRaw(rawArgs, sizeof...(Args), &rawResult)) {
						if constexpr (!std::is_void_v<R>) {
							return fromRawSlot<R>(rawResult);
						} else {
							return;
						}
					}
				}
				if constexpr (!std::is_void_v<R>) {
					std::vector<std::any> inputs_ = {getGenericArg(arguments)...};
					auto res = genericFunction->invokeGeneric(inputs_);
					return castGenericResult<R>(res);
				} else {
					std::vector<std::any> inputs_ = {getGenericArg(arguments)...};
					genericFunction->invokeGeneric(inputs_);
				}
			}
		}
	};

	/**
	 * @brief Returns an invokable function for the member.
	 * @tparam R return type
	 * @tparam Args types of arguments
	 * @param member function name
	 * @return Invocable
	 */
	template <typename R, typename... Args>
	auto getInvocableMember(const std::string& member) {
		if (hasInvocableFunctionPtr()) {
			return Invocable<R, Args...>(getInvocableFunctionPtr(member));
		} else {
			return Invocable<R, Args...>(getGenericInvocable(member));
		}
	}

	virtual ~Executable();

	/**
	 * @brief Returns a untyped function pointer to a specific symbol.
	 * @param member on the dynamic object, currently provided as a MangledName.
	 * @return function ptr
	 */
	[[nodiscard]] virtual void* getInvocableFunctionPtr(const std::string& member) = 0;

	/**
	 * @brief Determines if the executable can be called by a function ptr
	 * @return bool
	 */
	virtual bool hasInvocableFunctionPtr() = 0;

	/**
	 * @brief Returns an generic invocable function
	 * @return std::unique_ptr<GenericInvocable>
	 */
	virtual std::unique_ptr<GenericInvocable> getGenericInvocable(const std::string&) {
		return nullptr;
	}

	void setGeneratedFiles(const std::map<std::string, std::string>& generatedFiles);

	const std::string_view getGeneratedFile(std::string_view key) const;

	/// Attach statistics captured during the compilation that produced this
	/// executable. Callers pass a shared pointer so that the module can
	/// forward the same stats object to inspection APIs without copying.
	void setCompilationStatistics(std::shared_ptr<const CompilationStatistics> statistics);

	/// Statistics for the compilation that produced this executable, or a
	/// null shared_ptr if none were attached (e.g. backends that pre-date
	/// the stats infrastructure).
	[[nodiscard]] std::shared_ptr<const CompilationStatistics> getCompilationStatistics() const;

private:
	std::map<std::string, std::string> generatedFiles;
	std::shared_ptr<const CompilationStatistics> compilationStatistics;
};

} // namespace nautilus::compiler
