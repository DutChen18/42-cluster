#include "cluster.h"

cell_t *game_update(game_t *game, cell_t *cell)
{
	cell_t *next = cell->neighbors[game->gravity];

	if (next == NULL && cell->value == 0)
		return cell;
	if (next == NULL)
		return cell->neighbors[(game->gravity + 3) % 6];
	cell_t *dropped = game_update(game, next);
	if (cell->value == 0)
		return dropped;
	int tmp = cell->value;
	cell->value = dropped->value;
	dropped->value = tmp;
	return cell->neighbors[(game->gravity + 3) % 6];
}

void game_rotate(game_t *game, int gravity)
{
	game->gravity = gravity;
	for (int i = 0; i < game->cell_count; i++)
		game_update(game, &game->cells[i]);
}

void game_drop(game_t *game, int q, int r, int s, int value)
{
	cell_t *cell = game_get(game, q, r, s);
	cell->value = value;
	game_update(game, cell);
}
