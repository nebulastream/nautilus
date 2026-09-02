#pragma once

#include <string>

namespace nautilus::compiler::ir {

/**
 * @brief Every spelling we know for one callee, plus the two answers callers
 * actually want.
 *
 * The four fields are provenance, not alternatives: `mangled` is what the
 * dynamic linker knows, `demangled` is what a human reads, `custom` is what
 * someone chose (a NautilusFunction's name, or a plugin override), and
 * `minted` is what the function table assigned when nobody could supply
 * anything else.  Any of the first three may be empty -- a static callee that
 * `dladdr` cannot name has none of them -- which is why `minted` exists and is
 * never empty.
 *
 * Callers never pick a field.  They ask one of two questions:
 *
 *   get()          what a human should read.  Total, never empty.
 *   forEmission()  what a backend may paste into generated code.  Total,
 *                  always a valid identifier, always unique within one table.
 *
 * Keeping both resolutions here is the point: there is exactly one place that
 * decides what "the name" means, so no call site can pick the wrong spelling.
 *
 * @warning Nothing outside a printer may read the provenance fields directly,
 * and nothing at all may branch on get().  Callee identity is the code address
 * or the definition pointer -- never a name.  Branching on a name is the defect
 * class the function table exists to remove.
 */
class FunctionName {
public:
	FunctionName() = default;

	/// Provenance setters.  All are optional; the table fills in what it has.
	void setMangled(std::string mangled) {
		mangled_ = std::move(mangled);
	}
	void setDemangled(std::string demangled) {
		demangled_ = std::move(demangled);
	}
	void setCustom(std::string custom) {
		custom_ = std::move(custom);
	}

	/// Assigned by FunctionTable when the entry is interned.  Never empty
	/// afterwards, which is what makes get() total.
	void setMinted(std::string minted) {
		minted_ = std::move(minted);
	}

	/// Assigned by FunctionTable once the emission name has been sanitised
	/// and uniquified against every other entry.
	void setEmission(std::string emission) {
		emission_ = std::move(emission);
	}

	[[nodiscard]] const std::string& getMangled() const {
		return mangled_;
	}
	[[nodiscard]] const std::string& getDemangled() const {
		return demangled_;
	}
	[[nodiscard]] const std::string& getCustom() const {
		return custom_;
	}
	[[nodiscard]] const std::string& getMinted() const {
		return minted_;
	}

	/// The display name: what a human should read, in the IR text, a
	/// diagnostic or a GraphViz node.  Resolves custom -> demangled ->
	/// mangled -> minted, so it is total as long as the table has minted a
	/// name (which it does for every entry it creates).
	[[nodiscard]] const std::string& get() const {
		if (!custom_.empty()) {
			return custom_;
		}
		if (!demangled_.empty()) {
			return demangled_;
		}
		if (!mangled_.empty()) {
			return mangled_;
		}
		return minted_;
	}

	/// The emission name: what a backend may use as an identifier in
	/// generated source, as an MLIR symbol, or as a key in its own tables.
	/// Seeded from get() so real function names survive into generated code,
	/// then sanitised and uniquified by the table.
	[[nodiscard]] const std::string& forEmission() const {
		return emission_;
	}

	/// True once the table has finished interning this entry.  Only the
	/// verifier and assertions should need to ask.
	[[nodiscard]] bool isResolved() const {
		return !minted_.empty() && !emission_.empty();
	}

private:
	std::string mangled_;
	std::string demangled_;
	std::string custom_;
	std::string minted_;
	std::string emission_;
};

/// Rewrites @p seed into a valid identifier: every character outside
/// [A-Za-z0-9_] becomes '_', runs of underscores collapse, a leading digit is
/// prefixed, and the result is capped so a template instantiation cannot
/// produce a kilobyte-long symbol.  Returns an empty string only when @p seed
/// contains nothing usable at all.
std::string sanitizeIdentifier(const std::string& seed);

} // namespace nautilus::compiler::ir
