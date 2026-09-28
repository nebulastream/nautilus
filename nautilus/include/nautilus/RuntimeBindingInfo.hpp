#pragma once

#include <map>
#include <memory>
#include <string>

namespace nautilus {

namespace runtime_binding {

struct Entry {
	std::string identity;
	std::string type;
	std::string symbol;
	void* address;
};

} // namespace runtime_binding

template <typename T>
class RuntimeBinding;

class RuntimeBindings {
public:
	template <typename T>
	RuntimeBinding<T> bind(std::string identity, T* address);

	const std::map<std::string, std::shared_ptr<const runtime_binding::Entry>>& entries() const {
		return entries_;
	}

	std::string schema() const;

private:
	std::shared_ptr<const runtime_binding::Entry> add(std::string identity, std::string type, void* address);
	std::map<std::string, std::shared_ptr<const runtime_binding::Entry>> entries_;
};

} // namespace nautilus
