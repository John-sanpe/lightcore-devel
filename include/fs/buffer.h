/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FS_BUFFER_H_
#define _FS_BUFFER_H_

#include <types.h>
#include <bitflags.h>
#include <fsdev.h>

enum fscache_flag {
    BUFFER_VALID    = 0,
    BUFFER_DIRTY    = 1,
};

struct fscache {
    unsigned long flags;
};

GENERIC_STRUCT_BITOPS(fscache, struct fscache, flags);
GENERIC_STRUCT_FLAG(fscache, struct fscache, flags, valid, BUFFER_VALID);
GENERIC_STRUCT_FLAG(fscache, struct fscache, flags, dirty, BUFFER_DIRTY);



#endif  /* _FS_BUFFER_H_ */
