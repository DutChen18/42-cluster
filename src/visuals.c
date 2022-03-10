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

void	hexagon_border_init(visuals_t *visuals, hexagon_t *obj, int width, int height, int color)
{
	const int border_size = get_border_size(height);

	obj->width = width;
	obj->height = height;
	obj->img = mlx_new_image(visuals->mlx, obj->width, obj->height);
	obj->color = color;
	for (int x = 0; x < obj->width; x++)
	{
		for (int y = 0; y < obj->height; y++)
		{
			int temp_y = abs(obj->height / 2 - y);
			int temp_x = (int) (obj->width / 2 - abs(obj->width / 2 - x)) * sqrt(3);
			if (temp_y <= temp_x)
			{
				if (temp_y > temp_x - border_size)
					mlx_put_pixel(obj->img, x, y, obj->color);
				else
					mlx_put_pixel(obj->img, x, y, 0x333333FF);
			}
		}
	}
	for(int y = 0; y < border_size / 2; y++)
	{
		for (int x = obj->width / 4; x < obj->width / 4 * 3; x++)
		{
			mlx_put_pixel(obj->img, x, y, obj->color);
			mlx_put_pixel(obj->img, x, y + obj->height - border_size / 2, obj->color);
		}
	}
}

void place_border(visuals_t *visuals, cell_t *cell, hexagon_t *texture)
{
	int index;
	const int border_size = get_border_size(visuals->cell_height);
	int x = (int) (visuals->cell_height - border_size / 2) * cell->x + (int) (WINDOW_WIDTH / 2 - texture->width / 2);
	int y = (int) (visuals->cell_height - border_size / 2) * cell->y + (int) (WINDOW_HEIGHT / 2 - texture->height / 2);
	index = mlx_image_to_window(visuals->mlx, texture->img, x, y);
	if (cell->q % 2 == 0)
		texture->img->instances[index].z = GRID_EVEN;
	else
		texture->img->instances[index].z = GRID_ODD;
}

void	hexagon_init(mlx_t *mlx, hexagon_t *obj, int width, int height, int color)
{
	float				gradient;
	const long double	sqrt_3 = sqrt(3);

	obj->width = width / 5 * 4;
	obj->height = height / 5 * 4;
	obj->img = mlx_new_image(mlx, obj->width, obj->height);
	obj->color = color;
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

static void set_sizes_cells(visuals_t *visuals, int height, int width)
{
	visuals->cell_height = WINDOW_HEIGHT / height;
	visuals->cell_diagonal = WINDOW_WIDTH / width * 4;
	if (visuals->cell_height < get_height_from_width(visuals->cell_diagonal))
		visuals->cell_diagonal = get_width_from_height(visuals->cell_height);
	else
		visuals->cell_height = get_height_from_width(visuals->cell_height);
}

void grid_init(visuals_t *visuals, game_t *game)
{
	hexagon_border_init(visuals, &visuals->grid.one_cell, visuals->cell_diagonal, visuals->cell_height, 0x222222FF);
	visuals->grid.width = WINDOW_WIDTH;
	visuals->grid.height = WINDOW_HEIGHT;
	visuals->grid.grid = mlx_new_image(visuals->mlx, visuals->grid.width, visuals->grid.height);
	for (int i = 0; i < game->cell_count; i++)
		if (game->cells[i].wall == false)
			place_border(visuals, &game->cells[i], &visuals->grid.one_cell);

}

void set_bg_gradients(mlx_t* mlx, mlx_image_t **bg_gradients)
{
	float			gradient;
	float			angle;
	float			x2;
	float			y2;
	unsigned int	newcolor;

	for (int i = 0; i < 6; i++)
	{
		bg_gradients[i] = mlx_new_image(mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
		for (int y = 0; y < WINDOW_HEIGHT; y++)
		{
			for (int x = 0; x < WINDOW_WIDTH; x++)
			{
				angle = i * 3.14159 / 3;
				y2 = (float) y / WINDOW_HEIGHT - 0.5;
				x2 = (float) x / WINDOW_WIDTH - 0.5;
				gradient = -y2 * cos(angle) + x2 * sin(angle) + 0.5;
				if (gradient < 0)
					gradient = 0;
				else if (gradient > 1)
					gradient = 1;
				newcolor = (unsigned)(gradient * 153) << 24 | (unsigned)(gradient * 153) << 8 | (unsigned)(gradient * 255);
				mlx_put_pixel(bg_gradients[i], x, y, newcolor);
			}
		}
	}
}

void visuals_init(visuals_t *visuals, mlx_t *mlx, game_t *game)
{
	visuals->mlx = mlx;
	visuals->skip_next = false;
	set_sizes_cells(visuals, game->config->grid_size * 2 - 1, 4 + 6 * (game->config->grid_size - 1));
	visuals->hexa_tiles = malloc(sizeof(*visuals->hexa_tiles) * game->config->color_count);
	for (int i = 0; i < game->config->color_count; i++)
		hexagon_init(mlx, &visuals->hexa_tiles[i], visuals->cell_diagonal, visuals->cell_height, game->colors[i]);
}
