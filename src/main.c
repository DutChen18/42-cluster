#include "cluster.h"
#include "MLX42/MLX42.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

int create_color(int r, int g, int b, int t)
{
	return (r << 24 | g << 16 | b << 8 | t);
}

int get_height_from_width(int width)
{
	return (int) (width / 2) * sqrt(3);
}

int get_width_from_height(int height)
{
	return height / sqrt(3) * 2;
}

void place_hexagon(visuals_t *visuals, cell_t *cell)
{
	int x, y;
	float dir_x, dir_y;
	float normal;
	hexagon_t *hex = &visuals->hexa_tiles[cell->chip.value];

	dir_x = cell->x - cell->chip.x;
	dir_y = cell->y - cell->chip.y;
	normal = sqrt(dir_x * dir_x + dir_y * dir_y);
	if (normal > 1)
	{
		dir_x /= normal;
		dir_y /= normal;
	}

	if (cell->chip.placed == false)
	{
		x = (int) (visuals->cell_height - GRID_BORDER_SIZE / 2) * (cell->chip.x) + (int) (WINDOW_WIDTH / 2 - hex->width / 2);
		y = (int) (visuals->cell_height - GRID_BORDER_SIZE / 2) * (cell->chip.y) + (int) (WINDOW_HEIGHT / 2 - hex->height / 2);
		cell->chip.tile_index = mlx_image_to_window(visuals->mlx, hex->img, x, y);
		cell->chip.placed = true;
	}
	else
	{
		x = (int) (visuals->cell_height - GRID_BORDER_SIZE / 2) * (cell->chip.x + dir_x) + (int) (WINDOW_WIDTH / 2 - hex->width / 2);
		y = (int) (visuals->cell_height - GRID_BORDER_SIZE / 2) * (cell->chip.y + dir_y) + (int) (WINDOW_HEIGHT / 2 - hex->height / 2);
		visuals->hexa_tiles[cell->chip.value].img->instances[cell->chip.tile_index].x = x;
		visuals->hexa_tiles[cell->chip.value].img->instances[cell->chip.tile_index].y = y;
		cell->chip.x += dir_x;
		cell->chip.y += dir_y;
	}
}

bool move_hexagons(visuals_t *visuals, game_t *game)
{
	int	move_count = 0;

	for (int i = 0; i < game->cell_count; i++)
	{
		if (game->cells[i].chip.value != -1)
		{
			cell_t *cell = &game->cells[i];
			if (cell->chip.placed == true && cell->chip.x == cell->x && cell->chip.y == game->cells[i].y)
				continue ;
			place_hexagon(visuals, &game->cells[i]);
			move_count++;
		}
	}
	return (move_count > 0);
}

void set_background(visuals_t *visuals, int color)
{
	mlx_image_t	*image;

	image = mlx_new_image(visuals->mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
	set_bg_gradients(visuals->mlx, visuals->bg_gradients);
	for (int y = 0; y < WINDOW_HEIGHT; y++)
		for (int x = 0; x < WINDOW_WIDTH; x++)
			mlx_put_pixel(image, x, y, color);
	mlx_image_to_window(visuals->mlx, image, 0, 0);
	for (int i = 0; i < 6; i++)
	{
		int index = mlx_image_to_window(visuals->mlx, visuals->bg_gradients[i], 0, 0);
		visuals->bg_gradients[i]->instances[index].z = 1;
	}
}

void make_first_frame(visuals_t *visuals, game_t *game)
{
	set_background(visuals, 0x333333FF);
	grid_init(visuals, game);
	move_hexagons(visuals, game); //is niet nodig, nu nog voor het testen
	mlx_image_to_window(visuals->mlx, visuals->grid.grid, 0, 0);
}

static void	process_movement(mlx_key_data_t keydata, void* param)
{
	cluster_t *data;

	data = (cluster_t*)param;
	if (keydata.key == MLX_KEY_ESCAPE)
		exit(0);
	if (data->moving == false)
	{
		data->moving = true;
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

	if (data->visuals.skip_next)
	{
		data->visuals.skip_next = false;
		return;
	}
	if (data->needs_move)
	{
		data->needs_move = false;
		data->visuals.skip_next = true;
		char *sep = "";
		for (int i = 0; i < data->game.config->color_count; i++)
		{
			printf("%s%d", sep, data->game.chip_counts[i]);
			sep = " ";
		}
		printf("\n");
		data->winner = game_turn(&data->game);
		move_hexagons(&data->visuals, &data->game);
		data->time = 0;
		return;
	}

	data->time += data->visuals.mlx->delta_time;
	while (data->time > data->game.config->bot_speed)
	{
		data->time -= data->game.config->bot_speed;
		data->moving = move_hexagons(&data->visuals, &data->game);
		if (!data->moving && data->winner == -1)
		{
			data->needs_move = true;
			break;
		}
	}
	for (int i = 0; i < 6; i++)
		data->visuals.bg_gradients[i]->enabled = false;
	data->visuals.bg_gradients[data->game.gravity]->enabled = true;
}

int main(int argc, char **argv)
{
	cluster_t	data;
	mlx_t		*mlx;
	config_t	config;

	if (argc != 3) {
		fprintf(stderr, "usage: %s [player 1] [player 2]", argv[0]);
		return EXIT_FAILURE;
	}
	config_read(&config, "config.txt");
	game_init(&data.game, &config);
	data.winner = game_start(&data.game, argv[1], argv[2]);
	if (config.use_mlx) {
		mlx = mlx_init(WINDOW_WIDTH, WINDOW_HEIGHT, "cluster", 1);
		visuals_init(&data.visuals, mlx, &data.game);
		data.time = 0;
		data.needs_move = false;
		make_first_frame(&data.visuals, &data.game);
		mlx_key_hook(mlx, process_movement, &data);
		mlx_loop_hook(mlx, frame, &data);
		mlx_loop(mlx);
	} else {
		while (data.winner == -1) {
			data.winner = game_turn(&data.game);
		}
	}
	return EXIT_SUCCESS;
}
