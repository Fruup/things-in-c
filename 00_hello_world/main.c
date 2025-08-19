#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void test_rand()
{

	int32_t delta = 0;
	size_t N = 1e3;

	for (size_t j = 0; j < N; j++)
	{

		int32_t gt = 0, lt = 0;

		for (size_t i = 0; i < 1e4; i++)
		{
			if (rand() > RAND_MAX / 2)
				gt++;
			else
				lt++;
		}

		delta += gt - lt;
	}

	printf("average delta = %f\n", (double)delta / (double)N);
}

void test_rand_float()
{
	const size_t N = 1e4;
	double avg = 0;

	for (size_t i = 1; i < N; i++)
	{
		double x = 2 * (double)rand() / (double)RAND_MAX - 1;

		avg += (x - avg) / (double)i;
	}

	printf("avg = %f\n", avg);
}

int main()
{
	srand(time(0));

	test_rand_float();

	// printf("Hello, World!\n");

	return 0;
}
