#include "MLX42/MLX42.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	mlx_t *mlx;

	mlx = mlx_init(800, 600, "cluster", 1);
	puts("Hello, World!");
	return (EXIT_SUCCESS);
}
