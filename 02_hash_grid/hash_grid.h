#ifndef HASH_GRID_H
#define HASH_GRID_H

#include "../01b_hash_map_static/hash_map.h"
#include "../01b_hash_map_static/hash_fn.h"

#define INTEGER_T int32_t
#define REAL_T float

typedef INTEGER_T integer_t;
typedef REAL_T real_t;

typedef struct
{
	integer_t x;
	integer_t y;
} Cell;

typedef struct
{
	real_t x;
	real_t y;
} Point;

typedef struct
{
	uint8_t count;
	Point points[32];
} CellPoints;

typedef struct
{
	HashMap map;
	real_t h; // cell size
} HashGrid;

HashGrid hash_grid_create(size_t capacity, real_t h);
void hash_grid_destroy(HashGrid *grid);

void hash_grid_add(HashGrid *grid, Point point);
void hash_grid_add_many(HashGrid *grid, Point *points, size_t count);

Cell hash_grid_get_cell(HashGrid *grid, real_t x, real_t y);
Cell hash_grid_get_cell_from_point(HashGrid *grid, Point p);

void hash_grid_get_cell_points(HashGrid *grid, Cell cell, CellPoints **cell_points_out);
void hash_grid_for_each_neighbor(HashGrid *grid, Point point, void (*fn)(CellPoints *, void *user_data), void *user_data);
void hash_grid_for_each_neighbor_cell(HashGrid *grid, Cell cell, void (*fn)(CellPoints *, void *user_data), void *user_data);

#endif
