#!/bin/bash
# Script to regenerate LLVM IR test reference files for Nautilus
#
# Usage:
#   ./s/regenerate-ir-references.sh [--help]
#
# WARNING: This script will delete and regenerate all LLVM IR reference files!

set -e  # Exit on error

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Script directory and project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"

# LLVM IR test directory
LLVM_IR_TEST_DIR="${PROJECT_ROOT}/nautilus/test/llvm-ir-test/reference-ir"

# Parse command line arguments
for arg in "$@"; do
	case $arg in
		--help)
			echo "Usage: $0 [--help]"
			echo ""
			echo "Regenerate LLVM IR test reference files for Nautilus"
			echo ""
			echo "This script will:"
			echo "  1. Delete all .ll files from test/llvm-ir-test/reference-ir/"
			echo "  2. Run LLVM IR tests to regenerate the reference files"
			echo ""
			echo "Options:"
			echo "  --help    Show this help message"
			exit 0
			;;
		*)
			echo -e "${RED}Error: Unknown option: $arg${NC}"
			echo "Use --help for usage information"
			exit 1
			;;
	esac
done

# Header
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}LLVM IR Test Reference Regeneration${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Check if build directory exists
if [ ! -d "${BUILD_DIR}" ]; then
	echo -e "${YELLOW}Build directory not found. Creating and building...${NC}"
	mkdir -p "${BUILD_DIR}"
	cd "${BUILD_DIR}"
	cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_TESTS=ON ..
	cmake --build . --target nautilus
else
	echo -e "${GREEN}Build directory found: ${BUILD_DIR}${NC}"
	# Rebuild to ensure tests are up to date
	echo -e "${YELLOW}Rebuilding tests...${NC}"
	cd "${BUILD_DIR}"
	cmake --build . --target nautilus
fi

echo ""

# Delete LLVM IR reference files
echo -e "${YELLOW}Deleting LLVM IR reference files...${NC}"

if [ -d "${LLVM_IR_TEST_DIR}" ]; then
	count=$(find "${LLVM_IR_TEST_DIR}" -name "*.ll" 2>/dev/null | wc -l)
	if [ "${count}" -gt 0 ]; then
		echo -e "  Deleting ${count} .ll files from reference-ir/"
		find "${LLVM_IR_TEST_DIR}" -name "*.ll" -delete
	fi
	echo -e "${GREEN}Deleted ${count} LLVM IR reference files${NC}"
else
	echo -e "${YELLOW}LLVM IR reference directory does not exist (will be created during test run)${NC}"
fi
echo ""

# Run LLVM IR tests
echo -e "${YELLOW}Running LLVM IR tests to regenerate references...${NC}"
echo ""

cd "${BUILD_DIR}"

echo -e "${BLUE}Running LLVM IR tests${NC}"
if ctest --test-dir nautilus -R "LLVM IR Test" --output-on-failure; then
	echo -e "${GREEN}✓ LLVM IR tests completed${NC}"
else
	# Tests may fail on first run when creating new references
	echo -e "${YELLOW}⚠ LLVM IR tests completed (some tests may have been initialized)${NC}"
fi
echo ""

echo -e "${GREEN}LLVM IR test references regenerated${NC}"
echo ""

# Summary
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "${GREEN}✓ LLVM IR test references regenerated${NC}"
echo -e "  Location: ${LLVM_IR_TEST_DIR}"
echo ""
echo -e "${YELLOW}IMPORTANT: You should now run the tests again to verify all references are correct:${NC}"
echo -e "  cd ${BUILD_DIR}"
echo -e "  ctest --test-dir nautilus -R \"LLVM IR Test\" --output-on-failure"
echo ""
echo -e "${GREEN}Done!${NC}"
