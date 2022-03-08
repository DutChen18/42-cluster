#include "cluster.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int popen2(const char* path, player_t* player)
{
	int in[2], out[2];
	pipe(in);
	pipe(out);
	player->pid = fork();
	if (player->pid == 0)
	{
		close(in[STDOUT_FILENO]);
		close(out[STDIN_FILENO]);
		dup2(in[STDIN_FILENO], STDIN_FILENO);
		dup2(out[STDOUT_FILENO], STDOUT_FILENO);
		execl(path, "", NULL);
	}
	else
	{
		close(in[STDIN_FILENO]);
		close(out[STDOUT_FILENO]);
		player->in = fdopen(out[STDIN_FILENO], "r");
		setbuf(player->in, NULL);
		player->out = fdopen(in[STDOUT_FILENO], "w");
		setbuf(player->out, NULL);
	}
	return 0;
}

void game_start(game_t *game, player_t *players)
{
	fprintf(players[0].out, "init %d %d %d 0\n", game->color_count, game->cell_count / game->color_count, game->grid_size);
	fprintf(players[1].out, "init %d %d %d 1\n", game->color_count, game->cell_count / game->color_count, game->grid_size);
}

int game_take_random(game_t *game)
{
	int total = 0;
	int value = game->turn * game->color_count / 2;;
	for (int i = 0; i < game->color_count / 2; i += 1) {
		total += game->chip_counts[i + value];
	}
	if (total == 0)
		return -1;
	int index = rand() % total;
	while (index >= game->chip_counts[value]) {
		index -= game->chip_counts[value];
		value += 1;
	}
	return value;
}

int game_turn(game_t *game, player_t *players)
{
	int a, b;
	int q, r, s, value;
	char action[256];
	a = game_take_random(game);
	b = game_take_random(game);
	if (a == -1 || b == -1)
		return !game->turn;
	fprintf(players[game->turn].out, "chips %d %d\n", a, b);
	fscanf(players[game->turn].in, "%255s", action);
	if (strcmp(action, "rotate") == 0) {
		fscanf(players[game->turn].in, "%d", &value);
		if (value < 0 || value >= 6)
			return !game->turn;
		fprintf(players[!game->turn].out, "rotate %d\n", value);
		game_rotate(game, value);
	} else if (strcmp(action, "drop") == 1) {
		fscanf(players[game->turn].in, "%d %d %d %d", &q, &r, &s, &value);
		cell_t *cell = game_get(game, q, r, s);
		if (cell == NULL || cell->value != -1)
			return !game->turn;
		if (value < game->turn * game->color_count / 2)
			return !game->turn;
		if (value >= (game->turn + 1) * game->color_count / 2)
			return !game->turn;
		fprintf(players[!game->turn].out, "drop %d %d %d %d\n", q, r, s, value);
		game_drop(game, q, r, s, value);
	} else
		return !game->turn;
	int winner = game_winner(game);
	if (winner != -1)
		return winner * 2 / game->color_count;
	return -1;
}

void game_loop(const char *path1, const char *path2)
{
	game_t game;
	player_t players[2];
	int winner = -1;

	game_init(NULL, &game, 4, 1);
	popen2(path1, &players[0]);
	popen2(path2, &players[1]);
	game_start(&game, players);

	while (winner != -1) {
		winner = game_turn(&game, players);
		game.turn = !game.turn;
	}
}
