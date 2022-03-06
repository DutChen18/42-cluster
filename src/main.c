
#include "cluster.h"
#include "MLX42/MLX42.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

void	hexagon_init(mlx_t *mlx, struct hexagon *obj, int height, int color)
{
	obj->height = height;
	obj->width = height / sqrt(3) * 2;
	obj->img = mlx_new_image(mlx, obj->width, obj->height);
	obj->color = color;
	for (int x = 0; x < obj->width; x++)
		for (int y = 0; y < obj->height; y++)
		{
			int temp_y = abs(obj->height / 2 - y);
			int temp_x = (obj->width / 2 - abs(obj->width / 2 - x)) * sqrt(3);
			if (temp_y < temp_x)
				mlx_put_pixel(obj->img, x, y, obj->color);
		}
}

int main(void)
{
	mlx_t		*mlx;
	mlx_image_t	*image;
	struct hexagon	hexagon;

	game_t game;
	game_init(&game, 2);
	for (int i = 0; i < game.cell_count; i++)
	{
		printf("%d %d %d %f %f\n", game.cells[i].q, game.cells[i].r, game.cells[i].s, game.cells[i].x, game.cells[i].y);
	}
	mlx = mlx_init(WINDOW_WIDTH, WINDOW_HEIGHT, "cluster", 0);
	image = mlx_new_image(mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
	hexagon_init(mlx, &hexagon, 120, 0x640064FF);
	for (int i = 0; i < game.cell_count; i++)
	{
		mlx_image_to_window(mlx, hexagon.img, 120 * game.cells[i].x + 200, 120 * game.cells[i].y + 200);
	}
	mlx_loop(mlx);
	return (EXIT_SUCCESS);
}
