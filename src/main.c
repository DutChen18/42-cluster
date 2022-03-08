
#include "cluster.h"
#include "MLX42/MLX42.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

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

void	place_border(mlx_t *mlx, cell_t *cell, hexagon_t *hexagon, game_t *game)
{
	int x, y;

	x = (game->cell_height - GRID_BORDER_SIZE / 2) * cell->x + (WINDOW_WIDTH / 2 - hexagon->width / 2);
	y = (game->cell_height - GRID_BORDER_SIZE / 2) * cell->y + (WINDOW_HEIGHT / 2 - hexagon->height / 2);

	mlx_image_to_window(mlx, hexagon->img, x, y);
}

void	place_hexagon(mlx_t *mlx, cell_t *cell, hexagon_t *hexagon, game_t *game)
{
	int x, y;
	float dir_x, dir_y;
	float normal;
	mlx_instance_t *new_instance;

	dir_x = cell->x - cell->old_x;
	dir_y = cell->y - cell->old_y;
	normal = sqrt(dir_x * dir_x + dir_y * dir_y);
	if (normal > 1)
	{
		dir_x /= normal;
		dir_y /= normal;
	}
	printf("dir=> x:%f	y:%f\n", dir_x, dir_y);

	if (cell->placed == false)
	{
		x = (game->cell_height - GRID_BORDER_SIZE / 2) * (cell->old_x) + (WINDOW_WIDTH / 2 - hexagon->width / 2);
		y = (game->cell_height - GRID_BORDER_SIZE / 2) * (cell->old_y) + (WINDOW_HEIGHT / 2 - hexagon->height / 2);
		new_instance = mlx_image_to_window(mlx, hexagon->img, x, y);
		cell->image = hexagon->img;
		cell->tile_instance = new_instance - hexagon->img->instances;
		cell->placed = true;
	}
	else
	{
		x = (game->cell_height - GRID_BORDER_SIZE / 2) * (cell->old_x + dir_x) + (WINDOW_WIDTH / 2 - hexagon->width / 2);
		y = (game->cell_height - GRID_BORDER_SIZE / 2) * (cell->old_y + dir_y) + (WINDOW_HEIGHT / 2 - hexagon->height / 2);
		cell->image->instances[cell->tile_instance].x = x;
		cell->image->instances[cell->tile_instance].y = y;
		cell->old_x += dir_x;
		cell->old_y += dir_y;
	}
}

bool	move_hexagons(mlx_t *mlx, game_t *game)
{
	int	move_count = 0;

	for (int i = 0; i < game->cell_count; i++)
	{
		if (game->cells[i].value != -1)
		{
			cell_t *cell = &game->cells[i];
			if (cell->placed == true && cell->old_x == cell->x && cell->old_y == game->cells[i].y)
				continue ;
			place_hexagon(mlx, &game->cells[i], &game->hexa_tiles[game->cells[i].value], game);
			move_count++;
		}
	}
	return (move_count > 0);
}

void	grid_init(mlx_t* mlx, grid_t *obj, game_t *game)
{
	hexagon_border_init(mlx, &obj->one_cell, game->cell_diagonal, game->cell_height, 0x222222FF);
	obj->width = WINDOW_WIDTH;
	obj->height = WINDOW_HEIGHT;
	obj->grid = mlx_new_image(mlx, obj->width, obj->height);
	for (int i = 0; i < game->cell_count; i++)
		place_border(mlx, &game->cells[i], &obj->one_cell, game);
}

void	set_background(mlx_t* mlx, int color)
{
	mlx_image_t	*image;
	mlx_image_t	*bg_gradient; // Background gradient
	float gradient_y;
	float gradient_x;
	float gradient;

	image = mlx_new_image(mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
	bg_gradient = mlx_new_image(mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
	for (int y = 0; y < WINDOW_HEIGHT; y++)
	{
		gradient_y = y;
		for (int x = 0; x < WINDOW_WIDTH; x++)
		{
			gradient_x = x;
			gradient = (gradient_y / 1.7 + gradient_x) / (WINDOW_HEIGHT + WINDOW_WIDTH) * 255;
			mlx_put_pixel(image, x, y, color);
			mlx_put_pixel(bg_gradient, x, y, create_color(0x33, 0x33, 0xFF, gradient));
		}
	}
	mlx_image_to_window(mlx, image, 0, 0);
	mlx_image_to_window(mlx, bg_gradient, 0, 0);
}

void	make_first_frame(mlx_t *mlx, game_t *game, grid_t *grid)
{
	set_background(mlx, 0x333333FF);
	grid_init(mlx, grid, game);
	move_hexagons(mlx, game); //is niet nodig, nu nog voor het testen
	mlx_image_to_window(mlx, grid->grid, 0, 0);
}

// bool	any(int key)
// {
// 	return (key == MLX_KEY_KP_8 || key == MLX_KEY_KP_5 || key == MLX_KEY_KP_9 || key == MLX_KEY_KP_6 || key == MLX_KEY_KP_4 || key == MLX_KEY_KP_6);
// }

static void	process_movement(mlx_key_data_t keydata, void* param)
{
	cluster_t *data;

	data = (cluster_t*)param;
	if (data->moving == false)
	{
		data->moving = true;
		if (keydata.key == MLX_KEY_ESCAPE)
			exit(0);
		if (keydata.key == MLX_KEY_KP_8 && keydata.action == 1)
			game_rotate(&data->game, 0);
		else if (keydata.key == MLX_KEY_KP_5 && keydata.action == 1)
			game_rotate(&data->game, 3);
		else if (keydata.key == MLX_KEY_KP_9 && keydata.action == 1)
			game_rotate(&data->game, 1);
		else if (keydata.key == MLX_KEY_KP_6 && keydata.action == 1)
			game_rotate(&data->game, 2);
		else if (keydata.key == MLX_KEY_KP_4 && keydata.action == 1)
			game_rotate(&data->game, 4);
		else if (keydata.key == MLX_KEY_KP_7 && keydata.action == 1)
			game_rotate(&data->game, 5);
		data->time = 0;
	}
}

static void	frame(void *param)
{
	cluster_t	*data = (cluster_t*)param;

	data->time += data->mlx->delta_time;
	if (data->time > 0.1)
	{
		data->time = 0;
		data->moving = move_hexagons(data->mlx, &data->game);
		if (!data->moving && data->winner == -1)
		{
			data->winner = game_turn(&data->game, data->players);
			move_hexagons(data->mlx, &data->game);
			data->game.turn = !data->game.turn;
			data->time = 0;
		}
	}
}

int main(int argc, char **argv)
{

	cluster_t			data;

	data.mlx = mlx_init(WINDOW_WIDTH, WINDOW_HEIGHT, "cluster", 1);
	game_init(data.mlx, &data.game, SIZE, 4);
	popen2(argv[1], &data.players[0]);
	popen2(argv[2], &data.players[1]);
	game_start(&data.game, data.players);
	data.time = 0;
	data.winner = -1;
	make_first_frame(data.mlx, &data.game, &data.grid);
	mlx_key_hook(data.mlx, process_movement, &data);
	mlx_loop_hook(data.mlx, frame, &data);
	mlx_loop(data.mlx);
	return (EXIT_SUCCESS);
}
