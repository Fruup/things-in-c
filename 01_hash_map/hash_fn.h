#ifndef HASH_FN_H
#define HASH_FN_H

#include <stdint.h>
#include <stddef.h>

typedef size_t (*HashFn)(const void *key, size_t key_size);

size_t default_hash_fn(const void *key, size_t key_size);

#endif
