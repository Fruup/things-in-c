#include <stdio.h>
#include <assert.h>

#include "hash_grid.h"

void cell_print(Cell cell)
{
	printf("Cell(x = %d, y = %d)\n", cell.x, cell.y);
}

HashGrid hash_grid_create(size_t capacity, real_t h)
{
	HashGrid grid = {0};

	grid.h = h;
	grid.map = hash_map_create(
					.capacity = capacity,
					.key_size = sizeof(Cell),
					.value_size = sizeof(CellPoints));

	hash_map_print(&grid.map);

	return grid;
}

void hash_grid_destroy(HashGrid *grid)
{
	hash_map_destroy(&grid->map);
}

void hash_grid_add(HashGrid *grid, Point point)
{
	Cell cell = hash_grid_get_cell_from_point(grid, point);

	CellPoints *cell_points;
	hash_map_get(&grid->map, &cell, (void **)&cell_points);

	if (cell_points)
	{
		// TODO: handle this gracefully
		assert(cell_points->count < 32);

		cell_points->points[cell_points->count++] = point;
	}
	else
	{
		CellPoints value = {
				.count = 1,
				.points = {point},
		};

		hash_map_add(&grid->map, &cell, &value);
	}
}

void hash_grid_add_many(HashGrid *grid, Point *points, size_t count)
{
	for (size_t i = 0; i < count; i++)
		hash_grid_add(grid, *(points + i));
}

Cell hash_grid_get_cell(HashGrid *grid, real_t x, real_t y)
{
	Cell cell = {
			.x = (integer_t)(x / grid->h),
			.y = (integer_t)(y / grid->h),
	};

	if (cell.x < 0)
		cell.x--;
	if (cell.y < 0)
		cell.y--;

	return cell;
}

Cell hash_grid_get_cell_from_point(HashGrid *grid, Point p)
{
	return hash_grid_get_cell(grid, p.x, p.y);
}

void hash_grid_get_cell_points(HashGrid *grid, Cell cell, CellPoints **cell_points_out)
{
	hash_map_get(&grid->map, &cell, (void **)cell_points_out);
}

void hash_grid_for_each_neighbor(HashGrid *grid, Point point, void (*fn)(CellPoints *, void *user_data), void *user_data)
{
	Cell cell = hash_grid_get_cell_from_point(grid, point);
	return hash_grid_for_each_neighbor_cell(grid, cell, fn, user_data);
}

void hash_grid_for_each_neighbor_cell(HashGrid *grid, Cell cell, void (*fn)(CellPoints *, void *user_data), void *user_data)
{
	for (integer_t dx = -1; dx <= 1; dx++)
	{
		for (integer_t dy = -1; dy <= 1; dy++)
		{
			Cell c = {
					.x = cell.x + dx,
					.y = cell.y + dy,
			};

			CellPoints *cell_points = 0;
			hash_grid_get_cell_points(grid, cell, &cell_points);

			if (!cell_points)
				continue;

			fn(cell_points, user_data);
		}
	}
}
