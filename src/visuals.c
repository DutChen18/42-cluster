#include "cluster.h"
#include <math.h>
#include <stdlib.h>

int get_border_size(int height)
{
	int border_size;
	border_size = height / 16;
	if (border_size < 4)
		border_size = 4;
	if (border_size % 2 == 1)
		border_size++;
	return border_size;
}

void	hexagon_border_init(visuals_t *visuals, hexagon_t *obj, int width, int height, int background_color, int border_color)
{
	const int border_size = get_border_size(height);

	obj->width = width;
	obj->height = height;
	obj->img = mlx_new_image(visuals->mlx, obj->width, obj->height);
	obj->background_color = background_color;
	obj->border_color = border_color;
	for (int x = 0; x < obj->width; x++)
	{
		for (int y = 0; y < obj->height; y++)
		{
			int temp_y = abs(obj->height / 2 - y);
			int temp_x = (int) (obj->width / 2 - abs(obj->width / 2 - x)) * sqrt(3);
			if (temp_y <= temp_x)
			{
				if (temp_y > temp_x - border_size)
					mlx_put_pixel(obj->img, x, y, obj->border_color);
				else
					mlx_put_pixel(obj->img, x, y, obj->background_color);
			}
		}
	}
	for(int y = 0; y < border_size / 2; y++)
	{
		for (int x = obj->width / 4; x < obj->width / 4 * 3; x++)
		{
			mlx_put_pixel(obj->img, x, y, obj->border_color);
			mlx_put_pixel(obj->img, x, y + obj->height - border_size / 2, obj->border_color);
		}
	}
}

void place_border(config_t *config, visuals_t *visuals, cell_t *cell, hexagon_t *texture, int place)
{
	int index;
	const int border_size = get_border_size(visuals->cell_height);
	int x = (int) (visuals->cell_height - border_size / 2) * cell->x + (int) (config->window_width / 2 - texture->width / 2);
	int y = (int) (visuals->cell_height - border_size / 2) * cell->y + (int) (config->window_height / 2 - texture->height / 2);
	index = mlx_image_to_window(visuals->mlx, texture->img, x, y);
	texture->img->instances[index].z = place;
}

void	hexagon_init(mlx_t *mlx, hexagon_t *obj, int width, int height, int color)
{
	float				gradient;
	const long double	sqrt_3 = sqrt(3);

	obj->width = width / 5 * 4;
	obj->height = height / 5 * 4;
	obj->img = mlx_new_image(mlx, obj->width, obj->height);
	obj->background_color = color;
	for (int x = 0; x < obj->width; x++)
		for (int y = 0; y < obj->height; y++)
		{
			int temp_y = abs(obj->height / 2 - y);
			int temp_x = (int) (obj->width / 2 - abs(obj->width / 2 - x)) * sqrt_3;
			if (temp_y <= temp_x)
			{
				unsigned new_color = 0xFF;
				gradient = 1 - (float)(y + x) / (height + width) / 2 - 0.1;
				new_color |= (unsigned)((color & 0xFF000000) * gradient) & 0xFF000000;
				new_color |= (unsigned)((color & 0x00FF0000) * gradient) & 0x00FF0000;
				new_color |= (unsigned)((color & 0x0000FF00) * gradient) & 0x0000FF00;
				mlx_put_pixel(obj->img, x, y, new_color);
			}
		}
}

static void set_sizes_cells(config_t *config, visuals_t *visuals, int height, int width)
{
	visuals->cell_height = (float) (config->window_height) / height;
	visuals->cell_diagonal = (float) config->window_width / width * 4;
	if (visuals->cell_height < get_height_from_width(visuals->cell_diagonal))
		visuals->cell_diagonal = get_width_from_height(visuals->cell_height);
	else
		visuals->cell_height = get_height_from_width(visuals->cell_diagonal);
}


void place_color_gui(config_t *config, visuals_t *visuals, cell_t *cell, hexagon_t *texture, int color, float offset)
{
	int index, x, y;
	const int border_size = get_border_size(visuals->cell_height);
	x = (int) (visuals->cell_height - border_size / 2) * (cell->x + offset) + (int) (config->window_width / 2 - texture->width / 2);
	y = (int) (visuals->cell_height - border_size / 2) * cell->y + (int) (config->window_height / 2 - texture->height / 2);
	index = mlx_image_to_window(visuals->mlx, texture->img, x, y);
	if (cell->q % 2 == 0)
		texture->img->instances[index].z = GRID;
	else
		texture->img->instances[index].z = GRID;
	hexagon_t *hex = &visuals->hexa_tiles[color];
	x = (int) (visuals->cell_height - border_size / 2) * (cell->x + offset) + (int) (config->window_width / 2 - hex->width / 2);
	y = (int) (visuals->cell_height - border_size / 2) * cell->y + (int) (config->window_height / 2 - hex->height / 2);
	index = mlx_image_to_window(visuals->mlx, hex->img, x, y);
	hex->img->instances[index].z = HEXAGON;
}

void grid_init(visuals_t *visuals, game_t *game)
{
	hexagon_border_init(visuals, &visuals->grid.one_cell, visuals->cell_diagonal, visuals->cell_height, game->config->cell_bg_color, game->config->cell_border_color);
	visuals->grid.width = game->config->window_width;
	visuals->grid.height = game->config->window_height;
	visuals->grid.grid = mlx_new_image(visuals->mlx, visuals->grid.width, visuals->grid.height);
	for (int i = 0; i < game->cell_count; i++)
	{
		if (game->cells[i].wall == false)
			place_border(game->config, visuals, &game->cells[i], &visuals->grid.one_cell, GRID);
		if (game->cells[i].q == game->config->grid_size - 1 && abs(game->cells[i].r) < game->config->color_count / 2)
			place_color_gui(game->config, visuals, &game->cells[i], &visuals->grid.one_cell, game->config->color_count - 1 - abs(game->cells[i].r), 1.5);
		if (game->cells[i].q == -(game->config->grid_size - 1) && game->cells[i].s < game->config->color_count / 2)
			place_color_gui(game->config, visuals, &game->cells[i], &visuals->grid.one_cell, game->cells[i].s, -1.5);
	}
}

void set_bg_gradients(config_t *config, mlx_t* mlx, mlx_image_t **bg_gradients)
{
	float			gradient;
	float			angle;
	float			x2;
	float			y2;
	unsigned int	newcolor;
	int				intensity;

	intensity = 100;
	for (int i = 0; i < 6; i++)
	{
		bg_gradients[i] = mlx_new_image(mlx, config->window_width, config->window_height);
		for (int y = 0; y < config->window_height; y++)
		{
			for (int x = 0; x < config->window_width; x++)
			{
				angle = i * 3.14159 / 3;
				y2 = (float) y / config->window_height - 0.5;
				x2 = (float) x / config->window_width - 0.5;
				gradient = -y2 * cos(angle) + x2 * sin(angle) + 0.5;
				if (gradient < 0)
					gradient = 0;
				else if (gradient > 1)
					gradient = 1;
				newcolor = 0;
				newcolor |= ((unsigned)((config->bg_gradient_color & 0xFF0000) * gradient) & 0xFF0000);
				newcolor |= ((unsigned)((config->bg_gradient_color & 0x00FF00) * gradient) & 0x00FF00);
				newcolor |= ((unsigned)((config->bg_gradient_color & 0x0000FF) * gradient) & 0x0000FF);
				mlx_put_pixel(bg_gradients[i], x, y, newcolor << 8 | (unsigned)(gradient * 255));
			}
		}
	}
}

void visuals_init(visuals_t *visuals, mlx_t *mlx, game_t *game)
{
	visuals->mlx = mlx;
	visuals->skip_next = false;
	visuals->winner_str = NULL;
	set_sizes_cells(game->config, visuals, game->config->grid_size * 2 - 1, 16 + 6 * (game->config->grid_size - 1));
	visuals->hexa_tiles = malloc(sizeof(*visuals->hexa_tiles) * game->config->color_count);
	for (int i = 0; i < game->config->color_count; i++)
		hexagon_init(mlx, &visuals->hexa_tiles[i], visuals->cell_diagonal, visuals->cell_height, game->colors[i]);
}
