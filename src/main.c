
#include "cluster.h"
#include "MLX42/MLX42.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

int get_height_from_width(int width) {
	return width / 2 * sqrt(3);
}

int	get_width_from_height(int height) {
	return height / sqrt(3) * 2;
}

void	hexagon_border_init(mlx_t *mlx, hexagon_t *obj, int width, int height, int color)
{
	obj->width = width;
	obj->height = height;
	obj->img = mlx_new_image(mlx, obj->width, obj->height);
	obj->color = color;
	for (int x = 0; x < obj->width; x++)
	{
		for (int y = 0; y < obj->height; y++)
		{
			int temp_y = abs(obj->height / 2 - y);
			int temp_x = (obj->width / 2 - abs(obj->width / 2 - x)) * sqrt(3);
			if (temp_y <= temp_x && temp_y > temp_x - GRID_BORDER_SIZE)
				mlx_put_pixel(obj->img, x, y, obj->color);
		}
	}
	for(int y = 0; y < GRID_BORDER_SIZE / 2; y++)
	{
		for (int x = width / 4; x < width / 4 * 3; x++)
		{
			mlx_put_pixel(obj->img, x, y, obj->color);
			mlx_put_pixel(obj->img, x, y + obj->height - GRID_BORDER_SIZE / 2, obj->color);
		}
	}
}

void	place_cell(mlx_t *mlx, cell_t *cell, hexagon_t *hexagon)
{
	int x, y;

	printf("cell coor -> x:%f	y:%f\n", cell->x, cell->y);
	x = (hexagon->height - GRID_BORDER_SIZE / 2) * cell->x + (WINDOW_WIDTH / 2 - hexagon->width / 2);
	y = (hexagon->height - GRID_BORDER_SIZE / 2) * cell->y + (WINDOW_HEIGHT / 2 - hexagon->height / 2);

	mlx_image_to_window(mlx, hexagon->img, x, y);
}

void	place_cells(mlx_t *mlx, game_t *game)
{
	for (int i = 0; i < game->cell_count; i++)
	{
		if (game->cells[i].value != -1)
			place_cell(mlx, &game->cells[i], &game->hexa_tiles[game->cells[i].value]);
	}
}

void	grid_init(mlx_t* mlx, grid_t *obj, game_t *game)
{
	hexagon_border_init(mlx, &obj->one_cell, game->cell_diagonal, game->cell_height, 0x222222FF);
	obj->width = WINDOW_WIDTH;
	obj->height = WINDOW_HEIGHT;
	obj->grid = mlx_new_image(mlx, obj->width, obj->height);
	for (int i = 0; i < game->cell_count; i++)
		place_cell(mlx, &game->cells[i], &obj->one_cell);
}

void	set_background(mlx_t* mlx, int color)
{
	mlx_image_t	*image;

	image = mlx_new_image(mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
	for (int y = 0; y < WINDOW_HEIGHT; y++)
		for (int x = 0; x < WINDOW_WIDTH; x++)
			mlx_put_pixel(image, x, y, color);
	mlx_image_to_window(mlx, image, 0, 0);
}

int main(void)
{
	mlx_t		*mlx;
	grid_t		grid;
	game_t game;

	mlx = mlx_init(WINDOW_WIDTH, WINDOW_HEIGHT, "cluster", 1);
	game_init(mlx, &game, SIZE, 4);
	set_background(mlx, 0x333333FF);
	game_drop(&game, 0, 0, 0, 0);
	game_drop(&game, 0, 0, 0, 1);
	game_drop(&game, -2, -1, 3, 2);
	game_drop(&game, 0, 0, 0, 3);
	game_rotate(&game, 1);
	place_cells(mlx, &game);
	grid_init(mlx, &grid, &game);
	mlx_image_to_window(mlx, grid.grid, 0, 0);
	mlx_loop(mlx);
	return (EXIT_SUCCESS);
}
