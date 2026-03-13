#!/bin/bash

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TESTS_DIR="$ROOT/tests"
LIB="$ROOT/libft_malloc.so"
TMP_DIR="$TESTS_DIR/tmp"

mkdir -p "$TMP_DIR"

# ── 1. Build ──────────────────────────────────────────────────────────────────
echo "Building library..."
cd "$ROOT"
make fclean > /dev/null 2>&1
make 2>&1
if [ ! -f "$LIB" ]; then
    echo -e "${RED}[BUILD FAILED]${NC} libft_malloc.so not created"
    exit 1
fi
echo -e "${GREEN}[BUILD OK]${NC}"
echo ""

PASS=0
FAIL=0

# ── 2. Run one test ───────────────────────────────────────────────────────────
run_test() {
    local name="$1"
    local src="$2"
    local bin="$TMP_DIR/$name"

    # Compile
    if ! cc -Wall -Wextra -I"$ROOT/inc" "$src" "$LIB" -o "$bin" 2>"$TMP_DIR/${name}_compile.log"; then
        echo -e "${RED}[FAIL]${NC} $name — compile error:"
        cat "$TMP_DIR/${name}_compile.log"
        FAIL=$((FAIL + 1))
        return
    fi

    # Run with custom malloc via dyld (macOS)
    local output
    output=$(DYLD_INSERT_LIBRARIES="$LIB" DYLD_FORCE_FLAT_NAMESPACE=1 \
             "$bin" 2>&1)
    local exit_code=$?

    if [ $exit_code -ne 0 ]; then
        echo -e "${RED}[FAIL]${NC} $name — exit code $exit_code"
        [ -n "$output" ] && echo "  output: $output"
        FAIL=$((FAIL + 1))
    else
        echo -e "${GREEN}[PASS]${NC} $name"
        [ -n "$output" ] && echo "  $output"
        PASS=$((PASS + 1))
    fi
}

# ── 3. Tests ──────────────────────────────────────────────────────────────────
run_test "test0"        "$TESTS_DIR/test0.c"
run_test "test1"        "$TESTS_DIR/test1.c"
run_test "test2"        "$TESTS_DIR/test2.c"
run_test "test3"        "$TESTS_DIR/test3.c"
run_test "test4"        "$TESTS_DIR/test4.c"
run_test "test5"        "$TESTS_DIR/test5.c"
run_test "test_leak"    "$TESTS_DIR/test_leak.c"
run_test "test_realloc" "$TESTS_DIR/test_realloc.c"

# ── 4. Summary ────────────────────────────────────────────────────────────────
echo ""
echo "────────────────────────────────"
TOTAL=$((PASS + FAIL))
echo -e "Results: ${GREEN}$PASS${NC} / $TOTAL passed"
if [ $FAIL -gt 0 ]; then
    echo -e "${RED}$FAIL test(s) failed${NC}"
    exit 1
fi
echo -e "${GREEN}All tests passed!${NC}"

# Cleanup
rm -rf "$TMP_DIR"
