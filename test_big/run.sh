#!/bin/sh
export DYLD_LIBRARY_PATH=../
export LD_PRELOAD="../libft_malloc.so"
$@
