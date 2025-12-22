#!/bin/bash
# Script to regenerate trace test reference files for Nautilus
# This includes tracing, ir, and after_ssa reference files
#
# Usage:
#   ./s/regenerate-trace-references.sh [--help]
#
# WARNING: This script will delete and regenerate all trace reference files!

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

# Test data directory
TRACE_TEST_DATA_DIR="${PROJECT_ROOT}/nautilus/test/data"

# Parse command line arguments
for arg in "$@"; do
	case $arg in
		--help)
			echo "Usage: $0 [--help]"
			echo ""
			echo "Regenerate trace test reference files for Nautilus"
			echo ""
			echo "This script will:"
			echo "  1. Delete all .trace files from test/data/*/{tracing,ir,after_ssa}/"
			echo "  2. Run trace tests to regenerate the reference files"
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
echo -e "${BLUE}Trace Test Reference Regeneration${NC}"
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

# Delete trace test reference files
echo -e "${YELLOW}Deleting trace test reference files...${NC}"

categories=(
	"bool-tests"
	"cast-tests"
	"control-flow-tests"
	"enum-tests"
	"expression-tests"
	"loop-tests"
	"pointer-tests"
	"regressions"
	"runtime-call-tests"
	"static-loop-tests"
)

groups=(
	"tracing"
	"ir"
	"after_ssa"
)

file_count=0
for category in "${categories[@]}"; do
	for group in "${groups[@]}"; do
		dir="${TRACE_TEST_DATA_DIR}/${category}/${group}"
		if [ -d "${dir}" ]; then
			count=$(find "${dir}" -name "*.trace" 2>/dev/null | wc -l)
			if [ "${count}" -gt 0 ]; then
				echo -e "  Deleting ${count} files from ${category}/${group}"
				find "${dir}" -name "*.trace" -delete
				file_count=$((file_count + count))
			fi
		fi
	done
done

echo -e "${GREEN}Deleted ${file_count} trace reference files${NC}"
echo ""

# Run trace tests
echo -e "${YELLOW}Running trace tests to regenerate references...${NC}"
echo ""

cd "${BUILD_DIR}"

# List of trace test categories
test_patterns=(
	"Expression Trace Test"
	"Control-flow Trace Test"
	"Loop Trace Test"
	"Runtime Call Trace Test"
	"Enum Trace Test"
	"Pointer Trace Test"
	"Bool Trace Test"
	"Cast Trace Test"
	"Static Loop Trace Test"
	"Regression Trace Test"
)

for pattern in "${test_patterns[@]}"; do
	echo -e "${BLUE}Running: ${pattern}${NC}"
	if ctest --test-dir nautilus -R "^${pattern}$" --output-on-failure; then
		echo -e "${GREEN}✓ ${pattern} completed${NC}"
	else
		# Note: Tests will fail on first run when creating new references
		# This is expected behavior
		echo -e "${YELLOW}⚠ ${pattern} completed (some tests may have been initialized)${NC}"
	fi
	echo ""
done

echo -e "${GREEN}Trace test references regenerated${NC}"
echo ""

# Summary
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "${GREEN}✓ Trace test references regenerated${NC}"
echo -e "  Location: ${TRACE_TEST_DATA_DIR}"
echo ""
echo -e "${YELLOW}IMPORTANT: You should now run the tests again to verify all references are correct:${NC}"
echo -e "  cd ${BUILD_DIR}"
echo -e "  ctest --test-dir nautilus -R \"Trace Test\" --output-on-failure"
echo ""
echo -e "${GREEN}Done!${NC}"
