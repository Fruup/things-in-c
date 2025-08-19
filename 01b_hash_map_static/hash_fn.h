#ifndef HASH_FN_H
#define HASH_FN_H

#include <stdint.h>
#include <stddef.h>

typedef uint64_t (*HashFn)(const void *key, size_t key_size);

uint64_t dumb_hash_fn(const void *key, size_t key_size);
uint64_t hash_fn_fnv64(const void *key, size_t key_size);

#endif
