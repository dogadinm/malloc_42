#! /bin/sh
#
# run.sh
# Copyright (C) 2015 gbersac <gbersac@e2r3p18.42.fr>
#
# Distributed under terms of the MIT license.
#

#!/bin/sh
export DYLD_LIBRARY_PATH=../
export LD_PRELOAD="../libft_malloc.so"
$@
