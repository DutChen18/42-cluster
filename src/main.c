
#include "cluster.h"
#include "MLX42/MLX42.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	mlx_t		*mlx;
	mlx_image_t	*image;

	mlx = mlx_init(WINDOW_WIDTH, WINDOW_HEIGHT, "cluster", 1);
	image = mlx_new_image(mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
	for (int y = 0; y < 100; y++)
		for (int x = 0; x < 100; x++)
			mlx_put_pixel(image, x, y, 0xFF0000FF);
	for (int y = 300; y < 400; y++)
		for (int x = 400; x < 500; x++)
			mlx_put_pixel(image, x, y, 0xFF00FF);
	mlx_image_to_window(mlx, image, 0, 0);
	mlx_loop(mlx);
	return (EXIT_SUCCESS);
}
