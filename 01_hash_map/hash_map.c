#include "./hash_map.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// HashMap hash_map_create(size_t capacity, HashFn hash_fn)

HashMap hash_map_create_options(HashMapCreateOptions options)
{
	HashMap map = {0};

	map.capacity = options.capacity;
	map.hash_fn = options.hash_fn;

	map.population = calloc(map.capacity, sizeof(map.population[0]));
	map.keys = calloc(map.capacity, sizeof(map.keys[0]));
	map.key_sizes = calloc(map.capacity, sizeof(map.key_sizes[0]));
	map.values = calloc(map.capacity, sizeof(map.values[0]));
	map.value_sizes = calloc(map.capacity, sizeof(map.value_sizes[0]));

	return map;
}

void hash_map_destroy(HashMap *map)
{
	free(map->keys);
	free(map->key_sizes);
	free(map->values);
	free(map->value_sizes);
	free(map->population);

	for (size_t i = 0; i < map->capacity; i++)
	{
		if (map->population[i] == 0)
			continue;

		free(map->keys[i]);
		free(map->values[i]);
	}

	map->capacity = 0;
}

void hash_map_resize_if_needed(HashMap *map)
{
	if (map->size < map->capacity / 4)
		return;

	HashMap new_map = hash_map_create_options((HashMapCreateOptions){
			.capacity = map->capacity <= 0 ? 32 : map->capacity * 2,
			.hash_fn = map->hash_fn,
	});

	for (size_t i = 0; i < map->capacity; i++)
	{
		if (map->population[i] == 0)
			continue;

		hash_map_add(&new_map, map->keys[i], map->key_sizes[i], map->values[i], map->value_sizes[i]);
	}

	hash_map_destroy(map);
	*map = new_map;
}

void hash_map_print(HashMap *map)
{
	printf("HashMap (%p):\n", map);
	printf("  hash_fn = %p:\n", map->hash_fn);
	printf("  keys = %p:\n", map->keys);
	printf("  values = %p:\n", map->values);
	printf("  population = %p:\n", map->population);
	printf("  capacity = %zu:\n", map->capacity);
	printf("  size = %zu:\n", map->size);
	printf("  load = %.2f:\n", (float)map->size / (float)map->capacity);
}

void hash_map_add(HashMap *map, const void *key, size_t key_size, const void *value, size_t value_size)
{
	assert(map->hash_fn);
	assert(map->size < map->capacity);

	size_t index = map->hash_fn(key, key_size) % map->capacity;

	// Linear probing
	for (size_t i = 0; i < map->capacity && map->population[index] != 0; i++)
		index = (index + 1) % map->capacity;

#define new_key map->keys[index]
	new_key = malloc(key_size);
	memcpy(new_key, key, key_size);
	map->key_sizes[index] = key_size;

#define new_value map->values[index]
	new_value = malloc(value_size);
	memcpy(new_value, value, value_size);
	map->value_sizes[index] = value_size;

	map->population[index] = 1;

	map->size++;

	hash_map_resize_if_needed(map);
}

bool hash_map_get_index(HashMap *map, const void *key, size_t key_size, size_t *out_index)
{
	size_t index = map->hash_fn(key, key_size) % map->capacity;

	// Linear probing
	for (size_t i = 0; i < map->capacity; i++)
	{
		if (map->population[index] == 0)
			return false;

		if (memcmp(map->keys[index], key, key_size) == 0)
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

bool hash_map_has(HashMap *map, const void *key, size_t key_size)
{
	return hash_map_get_index(map, key, key_size, NULL);
}

void hash_map_get(HashMap *map, const void *key, size_t key_size, void **value_out, size_t *value_size_out)
{
	size_t index;
	if (!hash_map_get_index(map, key, key_size, &index))
	{
		if (value_out)
			*value_out = NULL;

		return;
	}

	if (value_out)
		*value_out = map->values[index];
	if (value_size_out)
		*value_size_out = map->value_sizes[index];
}

bool hash_map_remove(HashMap *map, const void *key, size_t key_size)
{
	size_t index;
	if (!hash_map_get_index(map, key, key_size, &index))
		return false;

	size_t index_to_move = index;
	size_t h1 = map->hash_fn(key, key_size);

	for (size_t i = 0; i < map->capacity; i++)
	{
		size_t next_index = (index_to_move + 1) % map->capacity;

		if (map->population[index_to_move] == 0)
			break;

		size_t h2 = map->hash_fn(map->keys[next_index], map->key_sizes[next_index]);
		if (h1 % map->capacity != h2 % map->capacity)
			break;

		// next
		index_to_move = next_index;
	}

	map->population[index_to_move] = 0;

	if (index_to_move != index)
	{
		// Move the last element to the deleted spot
		map->keys[index] = map->keys[index_to_move];
		map->key_sizes[index] = map->key_sizes[index_to_move];
		map->values[index] = map->values[index_to_move];
		map->value_sizes[index] = map->value_sizes[index_to_move];
	}

	map->size--;

	return true;
}
