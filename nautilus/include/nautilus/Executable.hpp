#pragma once

#include <any>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace nautilus::compiler {

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

		virtual ~GenericInvocable() = default;
	};

	/**
	 * @brief A typed invocable wrapper that is callable as a function and forwards arguments a concrete implementation.
	 * @tparam R return type
	 * @tparam Args agument types
	 */
	template <typename R, typename... Args>
	class Invocable {
	public:
		using FunctionType = R(Args...);

		explicit Invocable(void* fptr) : function(reinterpret_cast<FunctionType*>(fptr)) {}

		explicit Invocable(std::unique_ptr<GenericInvocable> generic) : function(std::move(generic)) {}

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
		/**
		 * @brief Invoke the function with a set of arguments
		 * @param arguments
		 * @return returns the result of the function if any
		 */
		R operator()(Args... arguments) {
			if (std::holds_alternative<FunctionType*>(function)) {
				auto fptr = std::get<FunctionType*>(function);
				if constexpr (!std::is_void_v<R>) {
					return fptr(std::forward<Args>(arguments)...);
				} else {
					fptr(std::forward<Args>(arguments)...);
				}
			} else {
				auto& genericFunction = std::get<std::unique_ptr<GenericInvocable>>(function);
				if constexpr (!std::is_void_v<R>) {
					std::vector<std::any> inputs_ = {getGenericArg(arguments)...};
					auto res = genericFunction->invokeGeneric(inputs_);
					if constexpr (std::is_same_v<R, char>) {
						return std::any_cast<int8_t>(res);
					} else if constexpr (std::is_pointer_v<R>) {
						return (R) std::any_cast<void*>(res);
					} else {
						return std::any_cast<R>(res);
					}
				} else {
					std::vector<std::any> inputs_ = {getGenericArg(arguments)...};
					genericFunction->invokeGeneric(inputs_);
				}
			}
		}

	private:
		std::variant<FunctionType*, std::unique_ptr<GenericInvocable>> function;
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
};

} // namespace nautilus::compiler
