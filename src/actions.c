#include "cluster.h"

static void swap(void *a_void, void *b_void, size_t size)
{
	unsigned char *a_char = a_void;
	unsigned char *b_char = b_void;

	for (size_t i = 0; i < size; i++)
	{
		a_char[i] ^= b_char[i];
		b_char[i] ^= a_char[i];
		a_char[i] ^= b_char[i];
	}
}

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
	swap(&cell->value, &dropped->value, sizeof(cell->value));
	swap(&cell->old_x, &dropped->old_x, sizeof(cell->old_x));
	swap(&cell->old_y, &dropped->old_y, sizeof(cell->old_y));
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
	coord_convert(&cell->old_x, &cell->old_y, q, r, s);
	game_update(game, cell);
}
