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
# run_test <name> <src> [ENV_VAR=val ...]
run_test() {
    local name="$1"
    local src="$2"
    shift 2
    local env_prefix="$*"
    local bin="$TMP_DIR/$name"

    # Compile (once per source; reuse binary if already built)
    if [ ! -f "$bin" ]; then
        if ! cc -Wall -Wextra -I"$ROOT/inc" "$src" "$LIB" -lpthread -o "$bin" \
                2>"$TMP_DIR/${name}_compile.log"; then
            echo -e "${RED}[FAIL]${NC} $name — compile error:"
            cat "$TMP_DIR/${name}_compile.log"
            FAIL=$((FAIL + 1))
            return
        fi
    fi

    local output
    output=$(env $env_prefix "$bin" 2>&1)
    local exit_code=$?

    if [ $exit_code -ne 0 ]; then
        echo -e "${RED}[FAIL]${NC} $name — exit code $exit_code"
        [ -n "$output" ] && echo "$output" | sed 's/^/    /'
        FAIL=$((FAIL + 1))
    else
        echo -e "${GREEN}[PASS]${NC} $name"
        [ -n "$output" ] && echo "$output" | sed 's/^/    /'
        PASS=$((PASS + 1))
    fi
}

# ── 3. Tests ──────────────────────────────────────────────────────────────────
echo -e "${YELLOW}── Core ──────────────────────────────${NC}"
run_test "test0"         "$TESTS_DIR/test0.c"
run_test "test1"         "$TESTS_DIR/test1.c"
run_test "test2"         "$TESTS_DIR/test2.c"
run_test "test3"         "$TESTS_DIR/test3.c"
run_test "test4"         "$TESTS_DIR/test4.c"
run_test "test5"         "$TESTS_DIR/test5.c"

echo ""
echo -e "${YELLOW}── Leak / Realloc / Coalesce ─────────${NC}"
run_test "test_leak"     "$TESTS_DIR/test_leak.c"
run_test "test_realloc"  "$TESTS_DIR/test_realloc.c"
run_test "test_coalesce" "$TESTS_DIR/test_coalesce.c"

echo ""
echo -e "${YELLOW}── Thread safety ─────────────────────${NC}"
run_test "test_threads"  "$TESTS_DIR/test_threads.c"

echo ""
echo -e "${YELLOW}── show_alloc_mem_ex ─────────────────${NC}"
run_test "test_show_ex"  "$TESTS_DIR/test_show_ex.c"

echo ""
echo -e "${YELLOW}── Debug env vars ────────────────────${NC}"
# No env vars: default behavior (no scribble)
run_test "test_debug_default"       "$TESTS_DIR/test_debug.c"
# MALLOC_VERBOSE: all calls logged
run_test "test_debug_verbose"       "$TESTS_DIR/test_debug.c"  MALLOC_VERBOSE=1
# MALLOC_PRE_SCRIBBLE: allocated memory filled with 0xAA
run_test "test_debug_pre_scribble"  "$TESTS_DIR/test_debug.c"  MALLOC_PRE_SCRIBBLE=1
# MALLOC_SCRIBBLE: freed memory filled with 0x55
run_test "test_debug_scribble"      "$TESTS_DIR/test_debug.c"  MALLOC_SCRIBBLE=1
# All three at once
run_test "test_debug_all"           "$TESTS_DIR/test_debug.c"  \
    MALLOC_VERBOSE=1 MALLOC_PRE_SCRIBBLE=1 MALLOC_SCRIBBLE=1

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
