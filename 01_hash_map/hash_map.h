#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "hash_fn.h"

typedef struct
{
	void **keys;
	size_t *key_sizes;

	void **values;
	size_t *value_sizes;

	uint8_t *population;

	size_t size;
	size_t capacity;

	HashFn hash_fn;
} HashMap;

typedef struct
{
	size_t capacity;
	HashFn hash_fn;
} HashMapCreateOptions;

HashMap hash_map_create_options(HashMapCreateOptions options);

#define hash_map_create(...)                      \
	hash_map_create_options((HashMapCreateOptions){ \
			.capacity = 32,                             \
			.hash_fn = hash_fn_fnv64,                   \
			__VA_ARGS__})

void hash_map_destroy(HashMap *map);
void hash_map_resize_if_needed(HashMap *map);
void hash_map_print(HashMap *map);

void hash_map_add(HashMap *map, const void *key, size_t key_size, const void *value, size_t value_size);
bool hash_map_get_index(HashMap *map, const void *key, size_t key_size, size_t *out_index);
bool hash_map_has(HashMap *map, const void *key, size_t key_size);
void hash_map_get(HashMap *map, const void *key, size_t key_size, void **value_out, size_t *value_size_out);
bool hash_map_remove(HashMap *map, const void *key, size_t key_size);

#endif
