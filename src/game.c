#include "cluster.h"
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

int init_player(const char* path, player_t* player)
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

void game_turn(game_t *game, player_t *players)
{
	int a = 2, b = 3;
	int q, r, s, value;
	char action[256];
	fprintf(players[game->turn].out, "chips %d %d\n", a, b);
	fscanf(players[game->turn].in, "%255s", action);
	if (strcmp(action, "rotate") == 0) {
		fscanf(players[game->turn].in, "%d", &value);
		fprintf(players[!game->turn].out, "rotate %d\n", value);
		game_rotate(game, value);
	} else if (strcmp(action, "drop") == 1) {
		fscanf(players[game->turn].in, "%d %d %d %d", &q, &r, &s, &value);
		fprintf(players[!game->turn].out, "drop %d %d %d %d\n", q, r, s, value);
		game_drop(game, q, r, s, value);
	}
}

void game_loop(const char *path1, const char *path2)
{
	game_t game;
	player_t players[2];

	game_init(&game, 4, 1);
	init_player(path1, &players[0]);
	init_player(path2, &players[1]);
	game_start(&game, players);

	while (1) {
		game_turn(&game, players);
		game.turn = !game.turn;
	}
}
