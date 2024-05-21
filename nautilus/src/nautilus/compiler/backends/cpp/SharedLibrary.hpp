#pragma once

#include <memory>
#include <string>

namespace nautilus::compiler::cpp {

class SharedLibrary;

using SharedLibraryPtr = std::shared_ptr<SharedLibrary>;

/**
 * @brief Represents a @DynamicObject, which relies on a shared library.
 */
class SharedLibrary {
public:
	/**
	 * @brief Creates a new @SharedLibrary Object
	 * @param shareLib
	 * @param soAbsolutePath: absolute path where so file is stored
	 */
	explicit SharedLibrary(void* shareLib, std::string soAbsolutePath);

	/**
	 * @brief Loads a shared library from a specific path.
	 * @param absoluteFilePath: absolute path where so file is stored
	 * @return SharedLibraryPtr
	 */
	static SharedLibraryPtr load(const std::string& absoluteFilePath);

	/**
	 * @brief Destructor for the shared library.
	 * Automatically unloads the shared library from memory.
	 */
	~SharedLibrary();

	/**
	 * @brief Returns a untyped function pointer to a specific symbol.
	 * @param member on the dynamic object, currently provided as a MangledName.
	 * @return function ptr
	 */
	[[nodiscard]] void* getInvocableFunctionPtr(const std::string& member);

private:
	void* shareLib;
	std::string soAbsolutePath;
};

} // namespace nautilus::compiler::cpp
