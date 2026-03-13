#!/bin/bash

# Leak check script for libft_malloc
# - macOS : uses 'leaks --atExit'
# - Linux : uses 'valgrind --pages-as-heap=yes'
#
# NOTE: --pages-as-heap tracks mmap/munmap directly, which is required
# for a custom allocator that bypasses system malloc entirely.

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TESTS_DIR="$ROOT/tests"
LIB="$ROOT/libft_malloc.so"
TMP_DIR="$TESTS_DIR/tmp_leak"

OS="$(uname -s)"

mkdir -p "$TMP_DIR"

# ── Build ──────────────────────────────────────────────────────────────────────
echo "Building library (DEBUG=yes for symbols)..."
cd "$ROOT"
make fclean > /dev/null 2>&1
make DEBUG=yes 2>&1 | tail -3
if [ ! -f "$LIB" ]; then
    echo -e "${RED}[BUILD FAILED]${NC}"
    exit 1
fi
echo -e "${GREEN}[BUILD OK]${NC}"
echo ""

PASS=0
FAIL=0

# ── Compile one test ───────────────────────────────────────────────────────────
compile_test() {
    local name="$1"
    local src="$2"
    local bin="$TMP_DIR/$name"

    cc -g -I"$ROOT/inc" "$src" "$LIB" -lpthread -o "$bin" \
        2>"$TMP_DIR/${name}_compile.log"
    if [ $? -ne 0 ]; then
        echo -e "${RED}[COMPILE ERROR]${NC} $name"
        cat "$TMP_DIR/${name}_compile.log"
        return 1
    fi
    echo "$bin"
}

# ── Run with leak checker ──────────────────────────────────────────────────────
run_leak() {
    local name="$1"
    local src="$2"
    local bin

    bin=$(compile_test "$name" "$src") || { FAIL=$((FAIL+1)); return; }

    echo -e "${YELLOW}── $name ──${NC}"

    if [ "$OS" = "Darwin" ]; then
        # macOS: leaks --atExit tracks allocations live via dyld
        # MallocStackLogging lets 'leaks' resolve stacks
        local output
        output=$(MallocStackLogging=1 leaks --atExit -- "$bin" 2>&1)
        local exit_code=$?

        # 'leaks' exits 0 if no leaks, 1 if leaks found
        local leaks_line
        leaks_line=$(echo "$output" | grep -E "^Process [0-9]+: [0-9]+ leaks")
        echo "$leaks_line"

        if echo "$leaks_line" | grep -q "^Process [0-9]*: 0 leaks"; then
            echo -e "${GREEN}[PASS]${NC} $name — no leaks"
            PASS=$((PASS+1))
        else
            echo -e "${RED}[FAIL]${NC} $name — leaks detected"
            echo "$output" | grep -A5 "LEAK:"
            FAIL=$((FAIL+1))
        fi

    elif [ "$OS" = "Linux" ]; then
        if ! command -v valgrind > /dev/null 2>&1; then
            echo -e "${YELLOW}[SKIP]${NC} valgrind not found"
            return
        fi
        local output
        output=$(valgrind \
            --leak-check=full \
            --show-leak-kinds=all \
            --pages-as-heap=yes \
            --error-exitcode=1 \
            "$bin" 2>&1)
        local exit_code=$?

        echo "$output" | grep -E "(ERROR SUMMARY|LEAK SUMMARY|definitely lost|indirectly lost)"

        if [ $exit_code -eq 0 ]; then
            echo -e "${GREEN}[PASS]${NC} $name — no leaks"
            PASS=$((PASS+1))
        else
            echo -e "${RED}[FAIL]${NC} $name — leaks or errors detected"
            FAIL=$((FAIL+1))
        fi
    else
        echo -e "${YELLOW}[SKIP]${NC} unsupported OS: $OS"
    fi
    echo ""
}

# ── Tests ──────────────────────────────────────────────────────────────────────
echo "Platform: $OS"
echo ""

run_leak "test_leak"     "$TESTS_DIR/test_leak.c"
run_leak "test_realloc"  "$TESTS_DIR/test_realloc.c"
run_leak "test_coalesce" "$TESTS_DIR/test_coalesce.c"

# ── Summary ───────────────────────────────────────────────────────────────────
echo "────────────────────────────────"
TOTAL=$((PASS + FAIL))
echo -e "Results: ${GREEN}$PASS${NC} / $TOTAL passed"
if [ $FAIL -gt 0 ]; then
    echo -e "${RED}$FAIL test(s) failed${NC}"
    rm -rf "$TMP_DIR"
    exit 1
fi
echo -e "${GREEN}All leak checks passed!${NC}"

rm -rf "$TMP_DIR"
