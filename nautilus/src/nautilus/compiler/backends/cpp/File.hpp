#pragma once

#include <memory>
#include <mutex>
#include <string>

namespace nautilus::compiler::cpp {

/**
 * @brief Represents for a file at a specific path
 */
class File {
public:
	/**
	 * @brief Constructor for a file at a specific path
	 * @param path
	 */
	explicit File(std::string path);

	/**
	 * @brief Creates a new file form a string.
	 * @param absoluteFilePath target absoluteFilePath of the file.
	 * @param content of the file
	 * @return std::shared_ptr<File>
	 */
	static std::shared_ptr<File> createFile(const std::string& absoluteFilePath, const std::string& content);

	/**
	 * @brief Returns the path of the file
	 * @return path
	 */
	std::string getPath() const;

	/**
	 * @brief Reads the file as a string
	 * @return file content
	 */
	std::string read() const;

	/**
	 * @brief Prints the file to the log
	 */
	void print() const;

	/**
	 * @brief Gives access to a mutex to lock the file
	 * @return std::mutex
	 */
	std::mutex& getFileMutex();

private:
	const std::string path;
	mutable std::mutex fileMutex;
};

} // namespace nautilus::compiler::cpp
