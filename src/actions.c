#include "cluster.h"

static void swap(void *a_void, void *b_void, size_t size)
{
	unsigned char *a_char = a_void;
	unsigned char *b_char = b_void;
	unsigned char tmp;

	for (size_t i = 0; i < size; i++)
	{
		tmp = a_char[i];
		a_char[i] = b_char[i];
		b_char[i] = tmp;
	}
}

cell_t *game_update(game_t *game, cell_t *cell)
{
	cell_t *next = cell->neighbors[game->gravity];

	if (next == NULL && cell->value == -1)
		return cell;
	if (next == NULL)
		return cell->neighbors[(game->gravity + 3) % 6];
	cell_t *dropped = game_update(game, next);
	if (cell->value == -1)
		return dropped;
	swap(&cell->value, &dropped->value, sizeof(cell->value));
	swap(&cell->old_x, &dropped->old_x, sizeof(cell->old_x));
	swap(&cell->old_y, &dropped->old_y, sizeof(cell->old_y));
	swap(&cell->tile_instance, &dropped->tile_instance, sizeof(cell->tile_instance));
	swap(&cell->placed, &dropped->placed, sizeof(cell->placed));
	swap(&cell->image, &dropped->image, sizeof(cell->image));
	return dropped->neighbors[(game->gravity + 3) % 6];
}

void game_rotate(game_t *game, int gravity)
{
	game->bg_gradients[game->gravity]->instances[0].z = -1;
	game->gravity = gravity;
	game->bg_gradients[game->gravity]->instances[0].z = 1;

	for (int i = 0; i < game->cell_count; i++)
		game_update(game, &game->cells[i]);
}

void game_drop(game_t *game, int q, int r, int s, int value)
{
	cell_t *cell = game_get(game, q, r, s);
	cell->value = value;
	cell->old_x = cell->x;
	cell->old_y = cell->y;
	game_update(game, cell);
}

int game_winner(game_t *game)
{
	int best_length = 0;
	int best_value = -1;
	int best_count = 0;

	for (int dir = 0; dir < 3; dir += 1) {
		for (int i = 0; i < game->cell_count; i += 1) {
			int length = 0;
			int value = game->cells[i].value;
			if (value == -1)
				continue;
			cell_t *cell = &game->cells[i];
			while (cell != NULL && cell->value == value) {
				length += 1;
				cell = cell->neighbors[dir];
			}
			if (length > best_length) {
				best_length = length;
				best_value = value;
				best_count = 1;
			} else if (length == best_length)
				best_count += 1;
		}
	}
	if (best_length >= 4 && best_count == 1)
		return best_value;
	return -1;
}
