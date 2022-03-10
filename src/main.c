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
	const int border_size = get_border_size(visuals->cell_height);
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
		x = (int) (visuals->cell_height - border_size / 2) * (cell->chip.x) + (int) (WINDOW_WIDTH / 2 - hex->width / 2);
		y = (int) (visuals->cell_height - border_size / 2) * (cell->chip.y) + (int) (WINDOW_HEIGHT / 2 - hex->height / 2);
		cell->chip.tile_index = mlx_image_to_window(visuals->mlx, hex->img, x, y);
		if (cell->q % 2 == 0)
			hex->img->instances[cell->chip.tile_index].z = HEXAGON_EVEN;
		else
			hex->img->instances[cell->chip.tile_index].z = HEXAGON_ODD;
		cell->chip.placed = true;
	}
	else
	{
		x = (int) (visuals->cell_height - border_size / 2) * (cell->chip.x + dir_x) + (int) (WINDOW_WIDTH / 2 - hex->width / 2);
		y = (int) (visuals->cell_height - border_size / 2) * (cell->chip.y + dir_y) + (int) (WINDOW_HEIGHT / 2 - hex->height / 2);
		hex->img->instances[cell->chip.tile_index].x = x;
		hex->img->instances[cell->chip.tile_index].y = y;
		if (dir_x != 0)
		{
			if (hex->img->instances[cell->chip.tile_index].z == HEXAGON_EVEN)
				hex->img->instances[cell->chip.tile_index].z = HEXAGON_ODD;
			else
				hex->img->instances[cell->chip.tile_index].z = HEXAGON_EVEN;
		}
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
	int index;
	mlx_image_t	*image;

	image = mlx_new_image(visuals->mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
	set_bg_gradients(visuals->mlx, visuals->bg_gradients);
	for (int y = 0; y < WINDOW_HEIGHT; y++)
		for (int x = 0; x < WINDOW_WIDTH; x++)
			mlx_put_pixel(image, x, y, color);
	index = mlx_image_to_window(visuals->mlx, image, 0, 0);
	image->instances[index].z = BACKROUND;
	for (int i = 0; i < 6; i++)
	{
		index = mlx_image_to_window(visuals->mlx, visuals->bg_gradients[i], 0, 0);
		visuals->bg_gradients[i]->instances[index].z = GRADIENTS;
	}
}

void	move_gui_cells(gui_t *obj, int color, int start, int end)
{
	for (int i = start; i < end; i++)
	{
		if (obj->colors[i].img->instances->z == 1)
			obj->colors[i].img->instances->z = DISMISS;
	}
	obj->colors[color].img->instances->z = obj->layer;
}

void	place_gui_cells(visuals_t *visuals, int color_count)
{
	int index;
	for (int i = 0; i < 4; i++)
	{
		index = mlx_image_to_window(visuals->mlx, visuals->gui[i].back_cell->img, visuals->gui[i].x, visuals->gui[i].y);
		if (i % 2 == 0)
			visuals->gui[i].back_cell->img->instances[index].z = GRID_EVEN;
		else
			visuals->gui[i].back_cell->img->instances[index].z = GRID_ODD;
		for (int j = 0; j < color_count; j++)
		{
			index = mlx_image_to_window(visuals->mlx, visuals->gui[i].colors[j].img, visuals->gui[i].x, visuals->gui[i].y);
			visuals->gui[i].colors[j].img->instances[index].z = DISMISS;
		}
	}
}

void	one_gui_cell(gui_t *obj, int x, int y, hexagon_t *colors, hexagon_t *back_cell, int layer)
{
	obj->x = x;
	obj->y = y;
	obj->colors = colors;
	obj->back_cell = back_cell;
	obj->layer = layer;
}

void	gui_init(visuals_t *visuals, config_t *config, game_t *game)
{
	int			x, y, mirror_x;
	const int	height = visuals->grid.height / 14;
	const int	width = get_width_from_height(height);
	const int	border_size = get_border_size(height);

	hexagon_t	*colors = malloc(sizeof(*colors) * config->color_count);
	hexagon_t	back_cell;
	for (int i = 0; i < config->color_count; i++)
		hexagon_init(visuals->mlx, &colors[i], width, height, game->colors[i]);
	hexagon_border_init(visuals, &back_cell, width, height, 0x222222FF);
	
	y = WINDOW_HEIGHT / 2 + visuals->cell_height * (config->grid_size - 0.5) - 1.25 * height;
	x = WINDOW_WIDTH / 2 - visuals->cell_diagonal * config->grid_size / 2;
	mirror_x = WINDOW_WIDTH - x - width;
	one_gui_cell(&visuals->gui[1], x, y, colors, &back_cell, HEXAGON_ODD);
	one_gui_cell(&visuals->gui[2], mirror_x, y, colors, &back_cell, HEXAGON_EVEN);

	x -= width / 4 * 3 - border_size / 4;
	y -= height / 2 - border_size / 4;
	mirror_x = WINDOW_WIDTH - x - width;
	one_gui_cell(&visuals->gui[0], x, y, colors, &back_cell, HEXAGON_EVEN);
	one_gui_cell(&visuals->gui[3], mirror_x, y, colors, &back_cell, HEXAGON_ODD);
	place_gui_cells(visuals, config->color_count);
}

void make_first_frame(visuals_t *visuals, game_t *game, config_t *config)
{
	set_background(visuals, 0x333333FF);
	grid_init(visuals, game);
	gui_init(visuals, config, game);
	mlx_image_to_window(visuals->mlx, visuals->grid.grid, 0, 0);
}

void	place_wall(game_t *game, int q, int r, int s)
{
	cell_t	*wall = game_get(game , q, r, s);
	
	for (int i = 0; i < 6; i++)
		if (wall->neighbors[i] != NULL)
			wall->neighbors[i]->neighbors[(i + 3) % 6] = NULL;
	wall->wall = true;
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
		// place_wall(&data.game, 2, 1, -3);
		// place_wall(&data.game, 2, 2, -4);
		// place_wall(&data.game, 3, -3, 0);
		make_first_frame(&data.visuals, &data.game, &config);
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
