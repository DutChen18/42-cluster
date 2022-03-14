#include "cluster.h"
#include "MLX42/MLX42.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

int get_height_from_width(int width)
{
	return (int) (width / 2) * sqrt(3);
}

int get_width_from_height(int height)
{
	return height / sqrt(3) * 2;
}

void place_hexagon(config_t *config, visuals_t *visuals, cell_t *cell)
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
		x = (int) (visuals->cell_height - border_size / 2) * (cell->chip.x) + (int) (config->window_width / 2 - hex->width / 2);
		y = (int) (visuals->cell_height - border_size / 2) * (cell->chip.y) + (int) (config->window_height / 2 - hex->height / 2);
		cell->chip.tile_index = mlx_image_to_window(visuals->mlx, hex->img, x, y);
		hex->img->instances[cell->chip.tile_index].z = HEXAGON;
		cell->chip.placed = true;
	}
	else
	{
		x = (int) (visuals->cell_height - border_size / 2) * (cell->chip.x + dir_x) + (int) (config->window_width / 2 - hex->width / 2);
		y = (int) (visuals->cell_height - border_size / 2) * (cell->chip.y + dir_y) + (int) (config->window_height / 2 - hex->height / 2);
		hex->img->instances[cell->chip.tile_index].x = x;
		hex->img->instances[cell->chip.tile_index].y = y;
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
			place_hexagon(game->config, visuals, &game->cells[i]);
			move_count++;
		}
	}
	return (move_count > 0);
}

void set_background(config_t *config, visuals_t *visuals)
{
	int index;
	mlx_image_t	*image;

	image = mlx_new_image(visuals->mlx, config->window_width, config->window_height);
	set_bg_gradients(config, visuals->mlx, visuals->bg_gradients);
	for (int y = 0; y < config->window_height; y++)
		for (int x = 0; x < config->window_width; x++)
		{
			mlx_put_pixel(image, x, y, config->bg_color << 8 | 0xFF);
		}
	index = mlx_image_to_window(visuals->mlx, image, 0, 0);
	image->instances[index].z = BACKROUND;
	for (int i = 0; i < 6; i++)
	{
		index = mlx_image_to_window(visuals->mlx, visuals->bg_gradients[i], 0, 0);
		visuals->bg_gradients[i]->instances[index].z = GRADIENTS;
	}
}

void	move_gui_cells(gui_t *obj, int color_count, int chip_a, int chip_b)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < color_count; j++)
			obj[i].colors[j].img->instances[i].z = DISMISS;
	int player = chip_a < (color_count / 2) ? 0 : 2;

	if (chip_a != -1)
		obj[player].colors[chip_a].img->instances[player].z = HEXAGON;
	if (chip_b != -1)
		obj[player + 1].colors[chip_b].img->instances[player + 1].z = HEXAGON;
}

void	place_gui_cells(visuals_t *visuals, int color_count)
{
	int index;
	for (int i = 0; i < 4; i++)
	{
		index = mlx_image_to_window(visuals->mlx, visuals->gui[i].back_cell->img, visuals->gui[i].x, visuals->gui[i].y);
		if (i % 2 == 0)
			visuals->gui[i].back_cell->img->instances[index].z = GRID;
		else
			visuals->gui[i].back_cell->img->instances[index].z = GRID;
		for (int j = 0; j < color_count; j++)
		{
			index = mlx_image_to_window(visuals->mlx, visuals->gui[i].colors[j].img, visuals->gui[i].x + visuals->gui[i].back_cell->width /8, visuals->gui[i].y + visuals->gui[i].back_cell->height / 8);
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

void	init_bag_count(bag_count_t *bag, int x, int y)
{
	bag->text = NULL;
	bag->x = x;
	bag->y = y;
}

void	gui_init(visuals_t *visuals, config_t *config, game_t *game)
{
	int			x, y, mirror_x;
	const int	height = visuals->grid.height / 14;
	const int	width = get_width_from_height(height);
	const int	border_size = get_border_size(height);

	hexagon_t	back_cell;
	hexagon_t	*colors = malloc(sizeof(*colors) * config->color_count);
	hexagon_border_init(visuals, &back_cell, width, height, config->cell_bg_color, config->cell_border_color);
	for (int i = 0; i < config->color_count; i++)
		hexagon_init(visuals->mlx, &colors[i], width, height, game->colors[i]);

	y = config->window_height / 2 + (int) ((int) (visuals->cell_height - get_border_size(visuals->cell_height) / 2) * (config->grid_size - 0.5)) - height;
	x = config->window_width / 2 - visuals->cell_diagonal * config->grid_size / 2;
	mirror_x = config->window_width - x - width;
	one_gui_cell(&visuals->gui[1], x, y, colors, &back_cell, HEXAGON);
	one_gui_cell(&visuals->gui[2], mirror_x, y, colors, &back_cell, HEXAGON);

	x -= width / 4 * 3 - border_size / 4;
	y -= height / 2 - border_size / 4;
	mirror_x = config->window_width - x - width;
	one_gui_cell(&visuals->gui[0], x, y, colors, &back_cell, HEXAGON);
	one_gui_cell(&visuals->gui[3], mirror_x, y, colors, &back_cell, HEXAGON);

	int grid_width = ((game->config->grid_size - 1) * 0.75 + 0.25) * visuals->cell_diagonal;
	x = game->config->window_width / 2 - grid_width;
	y = (int) (game->config->window_height / 2) + (visuals->cell_height * (game->config->grid_size - 0.5)) - 40;
	init_bag_count(&visuals->bag_counts[0], x, y);
	x = x + 2 * grid_width - 4 * 10;
	init_bag_count(&visuals->bag_counts[1], x, y);
	place_gui_cells(visuals, config->color_count);
}

void	put_exe_name(game_t *game, visuals_t *visuals)
{
	int x, y;

	int grid_width = ((game->config->grid_size - 1) * 0.75) * visuals->cell_diagonal;
	x = game->config->window_width / 2 - grid_width;
	y = (int) (game->config->window_height / 2) - (visuals->cell_height * (game->config->grid_size - 0.5));
	mlx_put_string(visuals->mlx, game->players[0].exe_name, x, y);
	x = x + 2 * grid_width - strlen(game->players[1].exe_name) * 10;
	mlx_put_string(visuals->mlx, game->players[1].exe_name, x, y);
}

void make_first_frame(visuals_t *visuals, game_t *game, config_t *config)
{
	set_background(game->config, visuals);
	grid_init(visuals, game);
	gui_init(visuals, config, game);
	mlx_image_to_window(visuals->mlx, visuals->grid.grid, 0, 0);
	put_exe_name(game, visuals);
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
	cluster_t	*data = (cluster_t*) param;

	if (keydata.key == MLX_KEY_ESCAPE)
		exit(0);
	if (data->needs_move && !data->game.players[data->game.turn].is_bot)
	{
		int direction = -1;
		if (keydata.key == MLX_KEY_W && keydata.action == 1)
			direction = 0;
		else if (keydata.key == MLX_KEY_S && keydata.action == 1)
			direction = 3;
		else if (keydata.key == MLX_KEY_E && keydata.action == 1)
			direction = 1;
		else if (keydata.key == MLX_KEY_D && keydata.action == 1)
			direction = 2;
		else if (keydata.key == MLX_KEY_A && keydata.action == 1)
			direction = 4;
		else if (keydata.key == MLX_KEY_Q && keydata.action == 1)
			direction = 5;
		if (direction != -1 && data->game.chip_a != -1 && data->game.chip_b != -1)
		{
			data->winner = game_postturn_rotate(&data->game, direction);
			data->needs_move = false;
			data->time = 0;
		}
	}
}

static cell_t *get_cell_pos(cluster_t *data, int *pos)
{
	int mx, my;
	float x, y;
	cell_t *cell = NULL;
	mlx_get_mouse_pos(data->visuals.mlx, &mx, &my);
	const int border_size = get_border_size(data->visuals.cell_height);
	x = (mx - data->game.config->window_width / 2.0) / (int) (data->visuals.cell_height - border_size / 2);
	y = (my - data->game.config->window_height / 2.0) / (int) (data->visuals.cell_height - border_size / 2);
	for (int i = 0; i < data->game.cell_count; i++) {
		cell_t *tmp = &data->game.cells[i];
		if (sqrtf(powf(x - tmp->x, 2.0f) + powf(y - tmp->y, 2.0f)) < 0.5f) {
			cell = tmp;
			break;
		}
	}
	if (cell == NULL)
		return NULL;
	switch (data->game.gravity) {
	case 0: *pos = -cell->q; break;
	case 3: *pos = cell->q; break;
	case 1: *pos = cell->s; break;
	case 4: *pos = -cell->s; break;
	case 2: *pos = -cell->r; break;
	case 5: *pos = cell->r; break;
	}
	return cell;
}

void	set_winning_line(visuals_t *visuals, game_t *game)
{
	hexagon_t winning_cell;

	// printf("starting printing line\n");
	hexagon_border_init(visuals, &winning_cell, visuals->cell_diagonal, visuals->cell_height, game->config->win_bg_color, game->config->win_border_color);
	for (int i = 0; i < game->cell_count; i++)
		if (game->cells[i].is_winning)
			place_border(game->config, visuals, &game->cells[i], &winning_cell, GRID_LINE);
}

static void	frame(void *param)
{
	cluster_t	*data = (cluster_t*)param;

	if (data->winner != -1 && data->game.config->autoclose)
	{
		exit(EXIT_SUCCESS);
	}
	if (data->visuals.skip_next)
	{
		data->visuals.skip_next = false;
		return;
	}
	if (data->needs_move && data->game.players[data->game.turn].is_bot)
	{
		data->needs_move = false;
		data->visuals.skip_next = true;
		data->winner = game_turn(&data->game);
		move_hexagons(&data->visuals, &data->game);
		data->time = 0;
	}

	if (mlx_is_mouse_down(data->visuals.mlx, MLX_MOUSE_BUTTON_LEFT)) {
		if (!data->left_state && data->needs_move && !data->game.players[data->game.turn].is_bot) {
			int pos;
			cell_t *cell = get_cell_pos(data, &pos);
			if (cell != NULL && cell->chip.value == -1 && data->game.chip_a != -1) {
				data->winner = game_postturn_drop(&data->game, cell->q, cell->r, cell->s, pos, data->game.chip_a);
				data->needs_move = false;
				data->time = 0;
			}
		}
		data->left_state = true;
	} else {
		data->left_state = false;
	}

	if (mlx_is_mouse_down(data->visuals.mlx, MLX_MOUSE_BUTTON_RIGHT)) {
		if (!data->right_state && data->needs_move && !data->game.players[data->game.turn].is_bot) {
			int pos;
			cell_t *cell = get_cell_pos(data, &pos);
			if (cell != NULL && cell->chip.value == -1 && data->game.chip_b != -1) {
				data->winner = game_postturn_drop(&data->game, cell->q, cell->r, cell->s, pos, data->game.chip_b);
				data->needs_move = false;
				data->time = 0;
			}
		}
		data->right_state = true;
	} else {
		data->right_state = false;
	}

	data->time += data->visuals.mlx->delta_time;
	while (data->time > data->game.config->bot_speed)
	{
		data->time -= data->game.config->bot_speed;
		data->moving = move_hexagons(&data->visuals, &data->game);
		if (!data->moving && data->winner == -1 && !data->needs_move)
		{
			for (int i = 0; i < 2; i++)
			{
				bag_count_t *bag = &data->visuals.bag_counts[i];
				if (bag->text != NULL)
					mlx_delete_image(data->visuals.mlx, bag->text);
				char buf[256];
				int total = 0;
				for (int j = 0; j < data->game.config->color_count / 2; j++)
					total += data->game.chip_counts[j + i * data->game.config->color_count / 2];
				sprintf(buf, "%04d", total);
				bag->text = mlx_put_string(data->visuals.mlx, buf, bag->x, bag->y);
			}
			data->winner = game_preturn(&data->game);
			move_gui_cells(data->visuals.gui, data->game.config->color_count, data->game.chip_a, data->game.chip_b);
			if (data->winner == -1)
				data->needs_move = true;
			break;
		}
	}

	for (int i = 0; i < 6; i++)
		data->visuals.bg_gradients[i]->enabled = false;
	data->visuals.bg_gradients[data->game.gravity]->enabled = true;

	if (data->winner != -1 && data->visuals.winner_str == NULL) {
		char winner_text[1024];
		sprintf(winner_text, "%s wins!", data->game.players[data->winner].exe_name);
		int winner_x = data->game.config->window_width / 2 - strlen(winner_text) * 5;
		if (data->winner == 0)
			data->visuals.winner_str = mlx_put_string(data->visuals.mlx, winner_text, winner_x, 10);
		if (data->winner == 1)
			data->visuals.winner_str = mlx_put_string(data->visuals.mlx, winner_text, winner_x, 10);
		set_winning_line(&data->visuals, &data->game);
	}
}

static void place_walls(game_t *game)
{
	srand(game->config->wall_seed);
	for (int i = 0; i < game->cell_count; i++) {
		cell_t *cell = &game->cells[i];
		int distance = (abs(cell->q) + abs(cell->r) + abs(cell->s)) / 2;
		if (distance + 1 != game->config->grid_size)
			if ((double) rand() / RAND_MAX < game->config->wall_chance)
				place_wall(game, cell->q, cell->r, cell->s);
	}
}

int main(int argc, char **argv)
{
	cluster_t	data;
	mlx_t		*mlx;
	config_t	config;

	config_read(&config, "config.txt", argv[0]);
	if (argc > 3 || (argc != 3 && !config.use_mlx)) {
		fprintf(stderr, "usage: %s [player 1] [player 2]", argv[0]);
		return EXIT_FAILURE;
	}
	game_init(&data.game, &config);
	place_walls(&data.game);
	if (argc == 2)
		data.winner = game_start(&data.game, NULL, argv[1]);
	else if (argc == 3)
		data.winner = game_start(&data.game, argv[1], argv[2]);
	else
		data.winner = game_start(&data.game, NULL, NULL);
	if (config.use_mlx) {
		mlx = mlx_init(config.window_width, config.window_height, "cluster", 1);
		visuals_init(&data.visuals, mlx, &data.game);
		data.time = 0;
		data.needs_move = false;
		data.left_state = false;
		data.right_state = false;
		make_first_frame(&data.visuals, &data.game, &config);
		mlx_key_hook(mlx, process_movement, &data);
		mlx_loop_hook(mlx, frame, &data);
		mlx_loop(mlx);
	} else {
		while (data.winner == -1) {
			data.winner = game_preturn(&data.game);
			if (data.winner == -1)
				data.winner = game_turn(&data.game);
		}
	}
	return EXIT_SUCCESS;
}
