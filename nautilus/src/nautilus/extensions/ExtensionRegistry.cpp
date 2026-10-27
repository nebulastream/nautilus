#include "nautilus/extensions/ExtensionRegistry.hpp"
#include "nautilus/nir/Inst.hpp"
#include "nautilus/nir/Types.hpp"
#include <algorithm>
#include <charconv>
#include <set>

namespace nautilus::ext {

namespace {

const std::set<std::string_view>& reservedWords() {
	static const std::set<std::string_view> words = [] {
		std::set<std::string_view> w;
		// every core mnemonic of 13 §5 and its first dot-separated component
		const char* mnemonics[] = {
#define NIR_OP(name, code, text, a, b, c, flags) text,
#include "nautilus/nir/Opcodes.def"
#undef NIR_OP
		};
		static std::vector<std::string> firsts;
		firsts.reserve(std::size(mnemonics));
		for (auto* m : mnemonics) {
			std::string_view s = m;
			w.insert(s);
			firsts.emplace_back(s.substr(0, s.find('.')));
			w.insert(firsts.back());
		}
		// type names of §2
		for (auto t : {"void", "bool", "i8", "i16", "i32", "i64", "f32", "f64", "ptr"}) {
			w.insert(t);
		}
		// the casts and comparison spellings that appear as mnemonic components, and the keywords of §9.2
		for (auto k : {"nir",      "module",    "extension", "import", "pointer", "binding", "data",     "callee",
		               "location", "region",    "func",      "export", "pressa",  "alloca",  "attr",     "debug",
		               "unwind",   "cleanup",   "thunk",     "prob",   "align",   "flags",   "internal", "external",
		               "imported", "intrinsic", "local",     "symbol", "image",   "audited", "raw",      "caller",
		               "null",     "pad",       "true",      "false",  "inf",     "nan",     "loc",      "name",
		               "at",       "in",        "size",      "params", "attrs",   "zext",    "sext",     "declared",
		               "cast",     "cmp",       "call",      "bind",   "div",     "rem",     "shr"}) {
			w.insert(k);
		}
		return w;
	}();
	return words;
}

bool isNameStartChar(char c) {
	return (c >= 'a' && c <= 'z') || c == '_';
}

} // namespace

bool splitNameVersion(std::string_view text, std::string_view& name, uint32_t& version) {
	auto at = text.rfind('@');
	if (at == std::string_view::npos || at == 0 || at + 1 >= text.size()) {
		return false;
	}
	uint32_t v = 0;
	auto digits = text.substr(at + 1);
	auto r = std::from_chars(digits.data(), digits.data() + digits.size(), v);
	if (r.ec != std::errc() || r.ptr != digits.data() + digits.size()) {
		return false;
	}
	name = text.substr(0, at);
	version = v;
	return true;
}

bool isValidPluginName(std::string_view plugin, std::string* why) {
	auto fail = [&](const char* reason) {
		if (why) {
			*why = reason;
		}
		return false;
	};
	if (plugin.empty() || !isNameStartChar(plugin[0])) {
		return fail("a plugin name starts with a lowercase letter or '_'");
	}
	for (char c : plugin) {
		if (!isNameStartChar(c) && !(c >= '0' && c <= '9')) {
			return fail("a plugin name has lowercase letters, digits and '_' only");
		}
	}
	if (reservedWords().count(plugin)) {
		return fail("the plugin name is a core mnemonic, a type name or a keyword of the text form");
	}
	return true;
}

ExtensionRegistry& ExtensionRegistry::instance() {
	static ExtensionRegistry registry;
	return registry;
}

void ExtensionRegistry::freeze() {
	std::lock_guard<std::mutex> lock(mutex_);
	locks_.fetch_add(1, std::memory_order_relaxed);
	frozen_.store(true, std::memory_order_release);
}

void ExtensionRegistry::checkOpen(std::string_view what, const std::string& name) const {
	// called with the mutex held
	if (frozen_.load(std::memory_order_acquire)) {
		throw RegistrationError(
		    "cannot register " + std::string(what) + " '" + name +
		    "': the extension registry is frozen. All extensions must be registered before the first "
		    "NautilusEngine is constructed (or the first NIR image is loaded or parsed). Do not "
		    "construct engines in static initializers.");
	}
}

void ExtensionRegistry::checkName(const std::string& name, Owner owner, const char* what) const {
	auto dot = name.find('.');
	if (dot == std::string::npos || dot + 1 == name.size()) {
		throw RegistrationError(std::string("invalid ") + what + " name '" + name + "': it is <plugin>.<name>");
	}
	std::string plugin = name.substr(0, dot);
	std::string why;
	if (plugin == "core") {
		if (owner != Owner::Core) {
			throw RegistrationError(std::string("cannot register ") + what + " '" + name +
			                        "': the prefix 'core' is reserved for the core's own intrinsics");
		}
		return;
	}
	if (owner == Owner::Core) {
		throw RegistrationError(std::string("the core registers only core.* names, not '") + name + "'");
	}
	if (!isValidPluginName(plugin, &why)) {
		throw RegistrationError(std::string("cannot register ") + what + " '" + name + "': " + why);
	}
}

uint8_t ExtensionRegistry::registerType(TypeInfo info, Owner owner) {
	std::lock_guard<std::mutex> lock(mutex_);
	locks_.fetch_add(1, std::memory_order_relaxed);
	checkOpen("type", info.name);
	checkName(info.name, owner, "type");
	if (types_.size() >= nir::MAX_EXTENSION_TYPES) {
		throw RegistrationError("cannot register type '" + info.name + "': at most 240 extension types");
	}
	if (info.parts == 0 || info.align == 0 || (info.align & (info.align - 1)) != 0) {
		throw RegistrationError("type '" + info.name + "' has invalid parts or alignment");
	}
	if (!byName_.emplace("type:" + info.name, static_cast<uint32_t>(types_.size())).second) {
		throw RegistrationError("type '" + info.name + "' is already registered");
	}
	types_.push_back(std::move(info));
	return static_cast<uint8_t>(16 + types_.size() - 1);
}

uint16_t ExtensionRegistry::registerOp(ExtOpInfo info, Owner owner) {
	std::lock_guard<std::mutex> lock(mutex_);
	locks_.fetch_add(1, std::memory_order_relaxed);
	checkOpen("operation", info.name);
	checkName(info.name, owner, "operation");
	if (info.targets == 0 && info.nativeFallback == nullptr && !info.hasExpansion) {
		throw RegistrationError("operation '" + info.name +
		                        "' has no fallback: a native function or an expansion is mandatory unless its targets "
		                        "are restricted");
	}
	if (!byName_.emplace("op:" + info.name, static_cast<uint32_t>(ops_.size())).second) {
		throw RegistrationError("operation '" + info.name + "' is already registered");
	}
	ops_.push_back(std::move(info));
	return static_cast<uint16_t>(nir::FIRST_EXTENSION_OP + ops_.size() - 1);
}

uint32_t ExtensionRegistry::registerIntrinsic(IntrinsicInfo info, Owner owner) {
	std::lock_guard<std::mutex> lock(mutex_);
	locks_.fetch_add(1, std::memory_order_relaxed);
	checkOpen("intrinsic", info.name);
	checkName(info.name, owner, "intrinsic");
	if (info.signature.empty()) {
		throw RegistrationError("intrinsic '" + info.name + "' has no signature");
	}
	if (info.fallback == nullptr) {
		throw RegistrationError("intrinsic '" + info.name + "' has no native fallback");
	}
	constexpr uint32_t noUnwind = 1u << 4;
	if ((info.attrs & noUnwind) == 0 && info.captureThunk == nullptr) {
		throw RegistrationError("intrinsic '" + info.name +
		                        "' may throw, so it needs a capture thunk (the registration helper instantiates it)");
	}
	auto id = static_cast<uint32_t>(intrinsics_.size());
	if (!byName_.emplace("intrinsic:" + info.name, id).second) {
		throw RegistrationError("intrinsic '" + info.name + "' is already registered");
	}
	byFallback_.emplace(info.fallback, id);
	intrinsics_.push_back(std::move(info));
	return id;
}

uint32_t ExtensionRegistry::registerPass(std::string name, Owner owner) {
	std::lock_guard<std::mutex> lock(mutex_);
	locks_.fetch_add(1, std::memory_order_relaxed);
	checkOpen("pass", name);
	checkName(name, owner, "pass");
	if (!byName_.emplace("pass:" + name, static_cast<uint32_t>(passes_.size())).second) {
		throw RegistrationError("pass '" + name + "' is already registered");
	}
	passes_.push_back(std::move(name));
	return static_cast<uint32_t>(passes_.size() - 1);
}

// Before the freeze a lookup takes the mutex (registration may be running); after it the tables are read-only and
// need no lock.
template <class F>
auto ExtensionRegistry::read(F&& f) const {
	if (frozen_.load(std::memory_order_acquire)) {
		return f();
	}
	std::lock_guard<std::mutex> lock(mutex_);
	locks_.fetch_add(1, std::memory_order_relaxed);
	return f();
}

const TypeInfo* ExtensionRegistry::type(uint8_t id) const {
	return read([&]() -> const TypeInfo* { return id >= 16 && id - 16u < types_.size() ? &types_[id - 16] : nullptr; });
}

const ExtOpInfo* ExtensionRegistry::op(uint16_t opcode) const {
	return read([&]() -> const ExtOpInfo* {
		return opcode >= nir::FIRST_EXTENSION_OP && size_t(opcode - nir::FIRST_EXTENSION_OP) < ops_.size()
		           ? &ops_[size_t(opcode - nir::FIRST_EXTENSION_OP)]
		           : nullptr;
	});
}

const IntrinsicInfo* ExtensionRegistry::intrinsic(uint32_t id) const {
	return read([&]() -> const IntrinsicInfo* { return id < intrinsics_.size() ? &intrinsics_[id] : nullptr; });
}

std::optional<uint8_t> ExtensionRegistry::findType(std::string_view name) const {
	return read([&]() -> std::optional<uint8_t> {
		auto it = byName_.find("type:" + std::string(name));
		return it == byName_.end() ? std::nullopt : std::optional<uint8_t>(static_cast<uint8_t>(16 + it->second));
	});
}

std::optional<uint16_t> ExtensionRegistry::findOp(std::string_view name) const {
	return read([&]() -> std::optional<uint16_t> {
		auto it = byName_.find("op:" + std::string(name));
		return it == byName_.end()
		           ? std::nullopt
		           : std::optional<uint16_t>(static_cast<uint16_t>(nir::FIRST_EXTENSION_OP + it->second));
	});
}

std::optional<uint32_t> ExtensionRegistry::findIntrinsic(std::string_view name) const {
	return read([&]() -> std::optional<uint32_t> {
		auto it = byName_.find("intrinsic:" + std::string(name));
		return it == byName_.end() ? std::nullopt : std::optional<uint32_t>(it->second);
	});
}

std::optional<uint32_t> ExtensionRegistry::findIntrinsicByFallback(const void* address) const {
	return read([&]() -> std::optional<uint32_t> {
		auto it = byFallback_.find(address);
		return it == byFallback_.end() ? std::nullopt : std::optional<uint32_t>(it->second);
	});
}

size_t ExtensionRegistry::typeCount() const {
	return read([&] { return types_.size(); });
}
size_t ExtensionRegistry::opCount() const {
	return read([&] { return ops_.size(); });
}
size_t ExtensionRegistry::intrinsicCount() const {
	return read([&] { return intrinsics_.size(); });
}

} // namespace nautilus::ext
