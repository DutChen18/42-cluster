#include "cluster.h"

cell_t *game_update(game_t *game, cell_t *cell)
{
	cell_t *next = cell->neighbors[game->gravity];
	chip_t chip_data;
	if (next == NULL && cell->chip.value == -1)
		return cell;
	if (next == NULL)
		return cell->neighbors[(game->gravity + 3) % 6];
	cell_t *dropped = game_update(game, next);
	if (cell->chip.value == -1)
		return dropped;
	chip_data = cell->chip;
	cell->chip = dropped->chip;
	dropped->chip = chip_data;
	return dropped->neighbors[(game->gravity + 3) % 6];
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
	cell->chip.value = value;
	cell->chip.x = cell->x;
	cell->chip.y = cell->y;
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
			int value = game->cells[i].chip.value;
			if (value == -1)
				continue;
			cell_t *cell = &game->cells[i];
			while (cell != NULL && cell->chip.value == value) {
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
	if (best_length >= game->config->win_length && best_count == 1)
		return best_value;
	return -1;
}