#include "./hash_map.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

HashMap hash_map_create_options(HashMapCreateOptions options)
{
	HashMap map = {0};

	map.capacity = options.capacity;
	map.hash_fn = options.hash_fn;
	map.key_size = options.key_size;
	map.value_size = options.value_size;

	map.population = malloc(map.capacity * sizeof(map.population[0]));
	memset(map.population, 0, map.capacity * sizeof(map.population[0]));

	map.keys = malloc(map.capacity * map.key_size);
	map.values = malloc(map.capacity * map.value_size);

	return map;
}

void hash_map_destroy(HashMap *map)
{
	free(map->keys);
	free(map->values);
	free(map->population);

	map->capacity = 0;
}

void hash_map_resize_if_needed(HashMap *map)
{
	if (map->size < map->capacity / 2)
		return;

	HashMap new_map = hash_map_create_options((HashMapCreateOptions){
			.capacity = map->capacity <= 0 ? 32 : map->capacity * 2,
			.hash_fn = map->hash_fn,
			.key_size = map->key_size,
			.value_size = map->value_size,
	});

	for (size_t i = 0; i < map->capacity; i++)
	{
		if (map->population[i] == 0)
			continue;

		hash_map_add(&new_map, map->keys + (i * map->key_size), map->values + (i * map->value_size));
	}

	hash_map_destroy(map);
	*map = new_map;
}

void hash_map_print(HashMap *map)
{
	printf("HashMap (%p):\n", map);
	printf("  hash_fn = %p:\n", map->hash_fn);
	printf("  keys = %p:\n", map->keys);
	printf("  key_size = %zu:\n", map->key_size);
	printf("  values = %p:\n", map->values);
	printf("  value_size = %zu:\n", map->value_size);
	printf("  population = %p:\n", map->population);
	printf("  capacity = %zu:\n", map->capacity);
	printf("  size = %zu:\n", map->size);
	printf("  load = %.2f:\n", (float)map->size / (float)map->capacity);
}

void hash_map_add(HashMap *map, const void *key, const void *value)
{
	assert(map->hash_fn);
	assert(map->size < map->capacity);

	size_t index = map->hash_fn(key, map->key_size) % map->capacity;

	// Linear probing
	for (size_t i = 0; i < map->capacity && map->population[index] != 0; i++)
		index = (index + 1) % map->capacity;

	memcpy(map->keys + (index * map->key_size), key, map->key_size);
	memcpy(map->values + (index * map->value_size), value, map->value_size);

	map->population[index] = 1;

	map->size++;

	hash_map_resize_if_needed(map);
}

bool hash_map_get_index(HashMap *map, const void *key, size_t *out_index)
{
	size_t index = map->hash_fn(key, map->key_size) % map->capacity;

	// Linear probing
	for (size_t i = 0; i < map->capacity; i++)
	{
		if (map->population[index] == 0)
			return false;

		if (memcmp(&map->keys[index], key, map->key_size) == 0)
		{
			if (out_index)
				*out_index = index;

			return true;
		}

		// next
		index = (index + 1) % map->capacity;
	}

	return false;
}

bool hash_map_has(HashMap *map, const void *key)
{
	return hash_map_get_index(map, key, NULL);
}

void hash_map_get(HashMap *map, const void *key, void **value_out)
{
	size_t index;
	if (!hash_map_get_index(map, key, &index))
	{
		if (value_out)
			*value_out = NULL;

		return;
	}

	if (value_out)
		*value_out = map->values + (index * map->value_size);
}

bool hash_map_remove(HashMap *map, const void *key)
{
	size_t index;
	if (!hash_map_get_index(map, key, &index))
		return false;

	size_t index_to_move = index;
	size_t h1 = map->hash_fn(key, map->key_size);

	for (size_t i = 0; i < map->capacity; i++)
	{
		size_t next_index = (index_to_move + 1) % map->capacity;

		if (map->population[index_to_move] == 0)
			break;

		size_t h2 = map->hash_fn(&map->keys[next_index], map->key_size);
		if (h1 % map->capacity != h2 % map->capacity)
			break;

		// next
		index_to_move = next_index;
	}

	map->population[index_to_move] = 0;

	if (index_to_move != index)
	{
		// Move the last element to the deleted spot
		memcpy(map->keys + (index * map->key_size), map->keys + (index_to_move * map->key_size), map->key_size);
		memcpy(map->values + (index * map->value_size), map->values + (index_to_move * map->value_size), map->value_size);
	}

	map->size--;

	return true;
}
