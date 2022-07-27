/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CRYPTO_BASE64_H_
#define _CRYPTO_BASE64_H_

#include <types.h>
#include <state.h>

static inline size_t base64_encode_length(size_t size)
{
    return (size + 2) / 3 * 4 + 1;
}

static inline size_t base64_decode_length(size_t size)
{
    return size / 4 * 3;
}

extern void base64_encode(void *buff, const void *data, size_t size);
extern state base64_decode(void *buff, const void *data);

#endif  /* _CRYPTO_BASE64_H_ */
