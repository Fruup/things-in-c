#include "hash_fn.h"

size_t default_hash_fn(const void *key, size_t key_size)
{
	size_t hash = 0;
	const uint8_t *data = (const uint8_t *)key;

	for (size_t i = 0; i < key_size; i++)
		hash += data[i];

	return hash;
}
