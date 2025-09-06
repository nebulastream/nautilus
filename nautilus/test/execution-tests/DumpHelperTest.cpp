#include "DumpHandler.hpp"
#include "nautilus/options.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>
#include <filesystem>
#include <iostream>

namespace nautilus {

// Tests if we only create a folder, if we need to
TEST_CASE("DumpHelperFolderCreationNoDumpToFile") {
	static const std::string folderName = "test_NoDumpToFile";
	const auto path = std::filesystem::temp_directory_path() / "dump" / folderName;
	if (std::filesystem::exists(path)) {
		std::filesystem::remove_all(path);
	}

	nautilus::engine::Options options;
	options.setOption("dump.toFile", false);
	const compiler::DumpHandler dumpHandler(options, folderName);
	dumpHandler.dump("after_ssa", "trace", []() { return "Some fancy text"; });
	dumpHandler.dump("after_ir_creation", "trace", []() { return "Some fancy text"; });
	dumpHandler.dump("after_mlir_generation", "trace", []() { return "Some fancy text"; });
	dumpHandler.dump("after_llvm_generation", "trace", []() { return "Some fancy text"; });
	dumpHandler.dump("after_cpp_generation", "trace", []() { return "Some fancy text"; });
	dumpHandler.dump("after_bc_generation", "trace", []() { return "Some fancy text"; });

	const auto dumpFolderExists = std::filesystem::exists(path);
	REQUIRE(dumpFolderExists == false);
}

// Tests if we only create a folder, if we need to
TEST_CASE("DumpHelperFolderCreationNoDumpAll") {
	static const std::string folderName = "test_NoDumpAll";
	const auto path = std::filesystem::temp_directory_path() / "dump" / folderName;
	if (std::filesystem::exists(path)) {
		std::filesystem::remove_all(path);
	}

	nautilus::engine::Options options;
	options.setOption("dump.all", false);
	const compiler::DumpHandler dumpHandler(options, folderName);
	dumpHandler.dump("after_ssa", "trace", []() { return "Some fancy text"; });
	dumpHandler.dump("after_ir_creation", "trace", []() { return "Some fancy text"; });
	dumpHandler.dump("after_mlir_generation", "trace", []() { return "Some fancy text"; });
	dumpHandler.dump("after_llvm_generation", "trace", []() { return "Some fancy text"; });
	dumpHandler.dump("after_cpp_generation", "trace", []() { return "Some fancy text"; });
	dumpHandler.dump("after_bc_generation", "trace", []() { return "Some fancy text"; });

	const auto dumpFolderExists = std::filesystem::exists(path);
	REQUIRE(dumpFolderExists == false);
}

// Tests if we only create a folder, if we need to
TEST_CASE("DumpHelperFolderCreationDumpAll") {
	static const std::string folderName = "test_DumpAll";
	const auto path = std::filesystem::temp_directory_path() / "dump" / folderName;
	if (std::filesystem::exists(path)) {
		std::filesystem::remove_all(path);
	}

	nautilus::engine::Options options;
	options.setOption("dump.all", true);
	options.setOption("dump.after_ssa", true);
	const compiler::DumpHandler dumpHandler(options, folderName);
	dumpHandler.dump("after_ssa", "trace", []() { return "Some fancy text"; });
	dumpHandler.dump("after_ir_creation", "trace", []() { return "Some fancy text"; });
	dumpHandler.dump("after_mlir_generation", "trace", []() { return "Some fancy text"; });
	dumpHandler.dump("after_llvm_generation", "trace", []() { return "Some fancy text"; });
	dumpHandler.dump("after_cpp_generation", "trace", []() { return "Some fancy text"; });
	dumpHandler.dump("after_bc_generation", "trace", []() { return "Some fancy text"; });

	const auto dumpFolderExists = std::filesystem::exists(path);
	REQUIRE(dumpFolderExists == true);
	std::filesystem::remove_all(path);
}

} // namespace nautilus
