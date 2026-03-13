# malloc

A custom dynamic memory allocator implementing `malloc`, `free`, and `realloc` using `mmap`/`munmap` system calls — a 42 school project.

## Overview

The allocator manages three tiers of memory zones to minimize system calls:

| Zone  | Allocation size     | Pre-allocated zone capacity |
|-------|---------------------|-----------------------------|
| TINY  | 1 – 1 024 bytes     | ≥ 100 chunks per zone       |
| SMALL | 1 025 – 32 768 bytes| ≥ 100 chunks per zone       |
| LARGE | > 32 768 bytes      | One allocation per `mmap`   |

Each zone size is aligned to a multiple of `getpagesize()`.

## Build

```bash
make          # builds libft_malloc_<HOSTTYPE>.so + libft_malloc.so symlink
make clean    # remove object files
make fclean   # remove objects + library
make re       # fclean + all
make DEBUG=yes # build with -g (no -Werror), useful for leak checks
```

`HOSTTYPE` is auto-detected via `uname -m`_`uname -s` if not set in the environment.

## Usage

Link directly against the library:

```bash
cc main.c ./libft_malloc.so -lpthread -o program
./program
```

Or inject at runtime without recompiling (macOS):

```bash
DYLD_INSERT_LIBRARIES=./libft_malloc.so DYLD_FORCE_FLAT_NAMESPACE=1 ./program
```

Linux equivalent:

```bash
LD_PRELOAD=./libft_malloc.so ./program
```

## API

```c
void  *malloc(size_t size);
void  *realloc(void *ptr, size_t size);
void   free(void *ptr);

void   show_alloc_mem(void);     // print all live allocations
void   show_alloc_mem_ex(void);  // same + hex dump of each allocation
```

Example output of `show_alloc_mem()`:

```
TINY : 0x102A00000
0x102A00060 - 0x102A0006A : 10 bytes
0x102A000A0 - 0x102A000C8 : 40 bytes
LARGE : 0x105000000
0x105000060 - 0x105100060 : 1048576 bytes
Total : 1048626 bytes
```

## Debug Environment Variables

Set before running your program — no recompilation needed.

| Variable            | Effect                                                  |
|---------------------|---------------------------------------------------------|
| `MALLOC_VERBOSE=1`  | Log every `malloc`, `realloc`, and `free` call to stdout |
| `MALLOC_SCRIBBLE=1` | Fill freed memory with `0x55` (detects use-after-free)  |
| `MALLOC_PRE_SCRIBBLE=1` | Fill allocated memory with `0xAA` (detects use of uninitialised memory) |

```bash
MALLOC_VERBOSE=1 ./program
MALLOC_SCRIBBLE=1 MALLOC_PRE_SCRIBBLE=1 ./program
```

## Thread Safety

All public functions are protected by a recursive `pthread` mutex initialized via a library constructor. Safe to use in multi-threaded programs without any additional setup.

## Testing

```bash
bash tests/run_test.sh   # full test suite (16 tests)
bash tests/run_leak.sh   # leak check (macOS: leaks, Linux: valgrind)
```

The test suite covers:
- Basic malloc / free / realloc correctness
- Alignment verification
- TINY / SMALL / LARGE allocation paths
- In-place realloc (grow, shrink, same size, type change)
- Chunk coalescing and memory defragmentation
- Thread safety (8–32 parallel threads)
- Debug environment variables

## Implementation Notes

- **Alignment** — returned pointers are aligned to `2 * sizeof(size_t)` (16 bytes on 64-bit).
- **Coalescing** — adjacent free chunks are merged on every `free` to reduce fragmentation.
- **Empty zones** — a zone is `munmap`'d as soon as all its chunks are freed.
- **Realloc optimisations** — tries in-place resize first; merges with the next free chunk if available; falls back to a new allocation only when necessary.
- **Global variables** — exactly two: `g_first_addr` (allocator state) and `g_malloc_mutex` (thread safety), as required by the project spec.

## Project Structure

```
.
├── inc/
│   └── malloc.h
├── src/
│   ├── malloc.c
│   ├── free.c
│   └── relloc.c
├── utilties/
│   ├── chunk.c
│   ├── map.c
│   ├── delete_chunk.c
│   ├── utilities_chunk.c
│   ├── utilities_map.c
│   ├── show_alloc_mem.c
│   ├── show_alloc_mem_ex.c
│   └── debug.c
├── libft/
│   ├── Makefile
│   └── ft_*.c
├── tests/
│   ├── run_test.sh
│   ├── run_leak.sh
│   └── test_*.c
└── Makefile
```
