#include "nautilus/tracing/TracingDiagnostics.hpp"
#include <cstdio>

namespace nautilus::tracing {

namespace {
std::string escapeJson(const std::string& text) {
	std::string out;
	for (const char c : text) {
		switch (c) {
		case '"':
			out += "\\\"";
			break;
		case '\\':
			out += "\\\\";
			break;
		case '\n':
			out += "\\n";
			break;
		case '\t':
			out += "\\t";
			break;
		default:
			if (static_cast<unsigned char>(c) < 0x20) {
				char buffer[8];
				std::snprintf(buffer, sizeof(buffer), "\\u%04x", c);
				out += buffer;
			} else {
				out += c;
			}
		}
	}
	return out;
}
} // namespace

std::string TracingDiagnostic::location() const {
	if (file.empty()) {
		return "<unknown location>";
	}
	return file + ":" + std::to_string(line);
}

std::string TracingDiagnostic::toText() const {
	return code + " " + title + " at " + location() + ": " + message + (hint.empty() ? "" : "; " + hint);
}

std::string TracingDiagnostic::toJson() const {
	return "{\"code\":\"" + escapeJson(code) + "\",\"title\":\"" + escapeJson(title) + "\",\"message\":\"" +
	       escapeJson(message) + "\",\"hint\":\"" + escapeJson(hint) + "\",\"file\":\"" + escapeJson(file) +
	       "\",\"line\":" + std::to_string(line) + ",\"function\":\"" + escapeJson(function) + "\"}";
}

DiagnosticsMode diagnosticsMode(const engine::Options& options) {
	const auto mode = options.getOptionOrDefault<std::string>("engine.diagnostics", "off");
	if (mode == "text") {
		return DiagnosticsMode::Text;
	}
	if (mode == "json") {
		return DiagnosticsMode::Json;
	}
	return DiagnosticsMode::Off;
}

void reportDiagnostic(const TracingDiagnostic& diagnostic, DiagnosticsMode mode) {
	if (mode == DiagnosticsMode::Off) {
		return;
	}
	const auto line = (mode == DiagnosticsMode::Json ? diagnostic.toJson() : diagnostic.toText()) + "\n";
	std::fwrite(line.data(), 1, line.size(), stderr);
}

} // namespace nautilus::tracing
