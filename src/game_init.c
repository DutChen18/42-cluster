#include "cluster.h"
#include <stdlib.h>
#include <math.h>

int	create_color(int r, int g, int b, int t)
{
	return (r << 24 | g << 16 | b << 8 | t);
}

void	hexagon_init(mlx_t *mlx, hexagon_t *obj, int width, int height, int color)
{
	long double sqrt_3 = sqrt(3);
	obj->width = width;
	obj->height = height;
	obj->img = mlx_new_image(mlx, obj->width, obj->height);
	obj->color = color;
	for (int x = 0; x < obj->width; x++)
		for (int y = 0; y < obj->height; y++)
		{
			int temp_y = abs(obj->height / 2 - y);
			int temp_x = (obj->width / 2 - abs(obj->width / 2 - x)) * sqrt_3;
			if (temp_y <= temp_x)
				mlx_put_pixel(obj->img, x, y, create_color((y + x) * -1 / 2, 0, 200, 255));
		}
	mlx_put_pixel(obj->img, obj->width/2, obj->height/2, 0x00FF00FF); //even voor het beeld
}

cell_t *game_get(game_t *game, int q, int r, int s)
{
	cell_t *cell;

	for (int i = 0; i < game->cell_count; i += 1) {
		cell = &game->cells[i];
		if (cell->q == q && cell->r == r && cell->s == s)
			return cell;
	}
	return NULL;
}

static void set_sizes_cells(game_t *game, int height, int width)
{
	game->cell_height = WINDOW_HEIGHT / height;
	game->cell_diagonal = WINDOW_WIDTH / width * 4;
	if (game->cell_height < get_height_from_width(game->cell_diagonal))
		game->cell_diagonal = get_width_from_height(game->cell_height);
	else
		game->cell_height = get_height_from_width(game->cell_height);
}

void	hexagon_color_array(mlx_t *mlx, game_t* game, int color_count)
{
	game->hexa_tiles = malloc(sizeof(*game->hexa_tiles) * 4);
	for (int i = 0; i < color_count; i++)
		hexagon_init(mlx, &game->hexa_tiles[i], game->cell_diagonal, game->cell_height, game->colors[i]);
}

void game_init(mlx_t *mlx, game_t *game, int size, int color_count)
{
	int		i = 0;
	cell_t	*cell;

	game->rings = size;
	set_sizes_cells(game, size * 2 - 1 ,4 + 6 * (size - 1));
	game->cell_count = (size * size - size) * 3 + 1;
	game->color_count = color_count;
	game->gravity = 3;
	game->grid_size = size;
	game->cells = malloc(sizeof(*game->cells) * game->cell_count);
	game->colors = malloc(sizeof(*game->colors) * game->color_count);
	game->chip_counts = malloc(sizeof(*game->chip_counts) * game->color_count);
	game->turn = 0;
	for (int i = 0; i < game->color_count; i += 1) {
		game->colors[i] = 0xFF;
		game->colors[i] |= 0xFFFF * ((i + 1) >> 0 & 1);
		game->colors[i] |= 0xFF00FF * ((i + 1) >> 1 & 1);
		game->colors[i] |= 0xFF0000FF * ((i + 1) >> 2 & 1);
		game->chip_counts[i] = game->cell_count / game->color_count;
	}
	if (mlx != NULL)
		hexagon_color_array(mlx, game, color_count);
	for (int q = -size + 1; q < size; q += 1) {
		for (int r = -size + 1; r < size; r += 1) {
			for (int s = -size + 1; s < size; s += 1) {
				if (q + r + s == 0) {
					cell = &game->cells[i];
					cell->q = q;
					cell->r = r;
					cell->s = s;
					coord_convert(&cell->x, &cell->y, q, r, s);
					coord_convert(&cell->old_x, &cell->old_y, q, r, s);
					game->cells[i].value = -1;
					i += 1;
				}
			}
		}
	}
	for (i = 0; i < game->cell_count; i += 1) {
		cell = &game->cells[i];
		cell->neighbors[0] = game_get(game, cell->q, cell->r - 1, cell->s + 1);
		cell->neighbors[1] = game_get(game, cell->q + 1, cell->r - 1, cell->s);
		cell->neighbors[2] = game_get(game, cell->q + 1, cell->r, cell->s - 1);
		cell->neighbors[3] = game_get(game, cell->q, cell->r + 1, cell->s - 1);
		cell->neighbors[4] = game_get(game, cell->q - 1, cell->r + 1, cell->s);
		cell->neighbors[5] = game_get(game, cell->q - 1, cell->r, cell->s + 1);
	}
}
