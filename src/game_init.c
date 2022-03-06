#include "cluster.h"
#include <stdlib.h>

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

void game_init(game_t *game, int size)
{
	int		i = 0;
	cell_t	*cell;

	game->cell_count = (size * size - size) * 3 + 1;
	game->cells = malloc(sizeof(*game->cells) * game->cell_count);
	for (int q = -size + 1; q < size; q += 1) {
		for (int r = -size + 1; r < size; r += 1) {
			for (int s = -size + 1; s < size; s += 1) {
				if (q + r + s == 0) {
					cell = &game->cells[i];
					cell->q = q;
					cell->r = r;
					cell->s = s;
					coord_convert(&cell->x, cell->y, q, r, s);
					coord_convert(&cell->old_x, cell->old_y, q, r, s);
					game->cells[i].value = 0;
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
		cell->neighbors[4] = game_get(game, cell->q - 1, cell->r, cell->s + 1);
		cell->neighbors[5] = game_get(game, cell->q - 1, cell->r + 1, cell->s);
	}
}
