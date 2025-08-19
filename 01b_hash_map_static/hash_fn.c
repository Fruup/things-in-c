#include "hash_fn.h"

uint64_t dumb_hash_fn(const void *key, size_t key_size)
{
	uint64_t hash = 0;
	const uint8_t *data = (const uint8_t *)key;

	for (size_t i = 0; i < key_size; i++)
		hash += data[i];

	return hash;
}

uint64_t hash_fn_fnv64(const void *key, size_t key_size)
{
	uint64_t prime = 0x00000100000001b3;
	uint64_t offset_basis = 0xcbf29ce484222325;

	uint64_t hash = offset_basis;
	uint8_t *data = (uint8_t *)key;

	for (size_t i = 0; i < key_size; i++)
	{
		hash *= prime;
		hash ^= (uint64_t)(data[i]);
	}

	return hash;
}
