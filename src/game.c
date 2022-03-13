#include "cluster.h"
#include <stdlib.h>
#include <math.h>

static void game_init_cells(game_t *game, config_t *config)
{
	int	i = 0;

	for (int q = -config->grid_size + 1; q < config->grid_size; q += 1) {
		for (int r = -config->grid_size + 1; r < config->grid_size; r += 1) {
			for (int s = -config->grid_size + 1; s < config->grid_size; s += 1) {
				if (q + r + s == 0) {
					cell_t *cell = &game->cells[i];
					cell->q = q;
					cell->r = r;
					cell->s = s;
					cell->wall = false;
					coord_convert(&cell->x, &cell->y, q, r, s);
					cell->chip.value = -1;
					cell->chip.tile_index = -1;
					cell->chip.placed = false;
					cell->chip.x = cell->x;
					cell->chip.y = cell->y;
					cell->is_winning = false;
					i += 1;
				}
			}
		}
	}
}

static void game_init_neighbors(game_t *game)
{
	for (int i = 0; i < game->cell_count; i += 1) {
		cell_t *cell = &game->cells[i];
		cell->neighbors[0] = game_get(game, cell->q, cell->r - 1, cell->s + 1);
		cell->neighbors[1] = game_get(game, cell->q + 1, cell->r - 1, cell->s);
		cell->neighbors[2] = game_get(game, cell->q + 1, cell->r, cell->s - 1);
		cell->neighbors[3] = game_get(game, cell->q, cell->r + 1, cell->s - 1);
		cell->neighbors[4] = game_get(game, cell->q - 1, cell->r + 1, cell->s);
		cell->neighbors[5] = game_get(game, cell->q - 1, cell->r, cell->s + 1);
	}
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

void game_init(game_t *game, config_t *config)
{
	game->config = config;
	game->cell_count = (config->grid_size * config->grid_size - config->grid_size) * 3 + 1;
	game->gravity = 3;
	game->cells = malloc(sizeof(*game->cells) * game->cell_count);
	game->colors = malloc(sizeof(*game->colors) * config->color_count);
	game->chip_counts = malloc(sizeof(*game->chip_counts) * config->color_count);
	game->turn = 0;
	for (int i = 0; i < config->color_count; i += 1) {
		game->colors[i] = 0xFF;
		game->colors[i] |= 0xFFFF * ((i + 1) >> 0 & 1);
		game->colors[i] |= 0xFF00FF * ((i + 1) >> 1 & 1);
		game->colors[i] |= 0xFF0000FF * ((i + 1) >> 2 & 1);
		game->chip_counts[i] = game->cell_count / config->color_count;
	}
	game_init_cells(game, config);
	game_init_neighbors(game);
}
