#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "hash_grid.h"

real_t random_real()
{
	return (real_t)rand() / (real_t)RAND_MAX;
}

typedef struct
{
	size_t visited_points;
} VisitorData;

void visitor(CellPoints *cell_points, void *data_)
{
	VisitorData *data = data_;
	data->visited_points += cell_points->count;
}

int main()
{
	srand(time(NULL));

	HashGrid grid = hash_grid_create(32, (real_t)0.1);

	const size_t n = 10000;
	const real_t spread = 3;

	for (size_t i = 0; i < n; i++)
	{
		Point point = (Point){
				.x = spread * (2.0 * random_real() - 1.0),
				.y = spread * (2.0 * random_real() - 1.0),
		};

		hash_grid_add(&grid, point);
	}

	hash_map_print(&grid.map);

	//
	for (size_t i = 0; i < grid.map.capacity; i++)
	{
		if (!grid.map.population[i])
			continue;

		Cell *cell = ((Cell *)grid.map.keys) + i;
		CellPoints *cell_points = ((CellPoints *)grid.map.values) + i;

		if (cell_points->count > 1)
			printf("Cell (%d, %d) contains %d points\n", cell->x, cell->y, cell_points->count);
	}

	//
	VisitorData data = {0};
	Point point = {.x = 0, .y = 0};
	hash_grid_for_each_neighbor(&grid, point, &visitor, &data);
	printf("# points visited = %zu\n", data.visited_points);

	hash_grid_destroy(&grid);
}