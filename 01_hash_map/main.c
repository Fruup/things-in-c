#include "hash_map.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

uint32_t key_1 = 1;
uint32_t key_2 = 2;
uint32_t key_3 = 3;
uint32_t key_4 = 4;

size_t value_1 = 100;
size_t value_2 = 200;
size_t value_3 = 300;
size_t value_4 = 400;

void test_0()
{
	HashMap map = hash_map_create(32, default_hash_fn);
	hash_map_print(&map);

	uint32_t key = 42;
	const char *value = "Hello, World!";
	hash_map_add(&map, &key, sizeof(key), &value, strlen(value));
	hash_map_print(&map);

	printf("has key = %d\n", hash_map_has(&map, &key, sizeof(key)));

	uint32_t key2 = 43;
	printf("has key2 = %d\n", hash_map_has(&map, &key2, sizeof(key2)));

	char **value_out;
	hash_map_get(&map, &key, sizeof(key), (void **)&value_out, NULL);
	printf("value_out = %s\n", *value_out);

	hash_map_remove(&map, &key, sizeof(key));
	hash_map_print(&map);

	hash_map_destroy(&map);
	hash_map_print(&map);
}

void test_resizing()
{
	HashMap map = hash_map_create(4, default_hash_fn);
	assert(map.size == 0);
	assert(map.capacity == 4);

	hash_map_add(&map, &key_1, sizeof(key_1), &value_1, sizeof(value_1));
	assert(map.size == 1);
	assert(map.capacity == 8);

	hash_map_add(&map, &key_2, sizeof(key_2), &value_2, sizeof(value_2));
	assert(map.size == 2);
	assert(map.capacity == 16);

	hash_map_add(&map, &key_3, sizeof(key_3), &value_3, sizeof(value_3));
	assert(map.size == 3);
	assert(map.capacity == 16);

	{
		assert(hash_map_has(&map, &key_1, sizeof(key_1)));

		void *value_out = NULL;
		size_t value_out_size = 0;
		hash_map_get(&map, &key_1, sizeof(key_1), &value_out, &value_out_size);

		assert(value_out);
		assert(value_out_size == sizeof(value_1));
		assert(memcmp(value_out, &value_1, value_out_size) == 0);
	}

	{
		assert(hash_map_has(&map, &key_2, sizeof(key_2)));

		void *value_out = NULL;
		size_t value_out_size = 0;
		hash_map_get(&map, &key_2, sizeof(key_2), &value_out, &value_out_size);

		assert(value_out);
		assert(value_out_size == sizeof(value_2));
		assert(memcmp(value_out, &value_2, value_out_size) == 0);
	}

	{
		assert(hash_map_has(&map, &key_3, sizeof(key_3)));

		void *value_out = NULL;
		size_t value_out_size = 0;
		hash_map_get(&map, &key_3, sizeof(key_3), &value_out, &value_out_size);

		assert(value_out);
		assert(value_out_size == sizeof(value_3));
		assert(memcmp(value_out, &value_3, value_out_size) == 0);
	}

	hash_map_destroy(&map);
}

void test_linear_probing()
{
	HashMap map = hash_map_create(32, default_hash_fn);
	assert(map.size == 0);

	uint32_t key_1_prime = key_1 + map.capacity;

	size_t h1 = map.hash_fn(&key_1, sizeof(key_1));
	size_t h1_prime = map.hash_fn(&key_1_prime, sizeof(key_1_prime));
	assert(h1_prime != h1);
	assert(h1_prime % map.capacity == h1 % map.capacity);

	hash_map_add(&map, &key_1, sizeof(key_1), &value_1, sizeof(value_1));
	hash_map_add(&map, &key_1_prime, sizeof(key_1_prime), &value_1, sizeof(value_1));
	assert(map.size == 2);

	size_t index_1, index_1_prime;
	hash_map_get_index(&map, &key_1, sizeof(key_1), &index_1);
	hash_map_get_index(&map, &key_1_prime, sizeof(key_1_prime), &index_1_prime);
	assert(index_1 != index_1_prime);

	// remove
	hash_map_remove(&map, &key_1, sizeof(key_1));
	assert(map.size == 1);

	hash_map_get_index(&map, &key_1_prime, sizeof(key_1_prime), &index_1_prime);
	assert(index_1_prime == index_1); // key_1_prime should have moved to the index of key_1

	hash_map_destroy(&map);
}

int main()
{
	test_resizing();
	test_linear_probing();
}
