#!/bin/bash
# Wrapper script to regenerate all test reference files for Nautilus
# This script calls the individual regeneration scripts
#
# Usage:
#   ./s/regenerate-test-references.sh [--trace-only|--ir-only|--help]
#
# Options:
#   --trace-only    Only regenerate trace test references
#   --ir-only       Only regenerate LLVM IR test references
#   --help          Show this help message

set -e  # Exit on error

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Flags
REGENERATE_TRACE=true
REGENERATE_IR=true

# Parse command line arguments
for arg in "$@"; do
	case $arg in
		--trace-only)
			REGENERATE_TRACE=true
			REGENERATE_IR=false
			shift
			;;
		--ir-only)
			REGENERATE_TRACE=false
			REGENERATE_IR=true
			shift
			;;
		--help)
			echo "Usage: $0 [--trace-only|--ir-only|--help]"
			echo ""
			echo "Regenerate test reference files for Nautilus"
			echo ""
			echo "This wrapper script calls:"
			echo "  - regenerate-trace-data.sh (for trace tests)"
			echo "  - regenerate-llvm-ir-data.sh (for LLVM IR tests)"
			echo ""
			echo "Options:"
			echo "  --trace-only    Only regenerate trace test references"
			echo "  --ir-only       Only regenerate LLVM IR test references"
			echo "  --help          Show this help message"
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
echo -e "${BLUE}Nautilus Test Reference Regeneration${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Run appropriate scripts
if [ "$REGENERATE_TRACE" = true ]; then
	echo -e "${YELLOW}Running trace reference regeneration...${NC}"
	"${SCRIPT_DIR}/regenerate-trace-data.sh"
	echo ""
fi

if [ "$REGENERATE_IR" = true ]; then
	echo -e "${YELLOW}Running IR reference regeneration...${NC}"
	"${SCRIPT_DIR}/regenerate-llvm-ir-data.sh"
	echo ""
fi

# Final summary
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}All Done!${NC}"
echo -e "${BLUE}========================================${NC}"

if [ "$REGENERATE_TRACE" = true ] && [ "$REGENERATE_IR" = true ]; then
	echo -e "${GREEN}✓ All test references regenerated${NC}"
elif [ "$REGENERATE_TRACE" = true ]; then
	echo -e "${GREEN}✓ Trace test references regenerated${NC}"
elif [ "$REGENERATE_IR" = true ]; then
	echo -e "${GREEN}✓ LLVM IR test references regenerated${NC}"
fi

echo ""
echo -e "${YELLOW}Run tests to verify:${NC}"
echo -e "  cd build"
echo -e "  ctest --test-dir nautilus --output-on-failure"
echo ""
