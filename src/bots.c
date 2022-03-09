#include "cluster.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <signal.h>

static void popen2(const char* path, player_t* player)
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
		exit(EXIT_FAILURE);
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
}

static int game_take_random(game_t *game)
{
	int total = 0;
	int value = game->turn * game->config->color_count / 2;
	for (int i = 0; i < game->config->color_count / 2; i += 1) {
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

static void handler(int sig)
{
	(void) sig;
}

void game_start(game_t *game, const char *p1, const char *p2)
{
	popen2(p1, &game->players[0]);
	popen2(p2, &game->players[1]);
	fprintf(game->players[0].out, "init %d %d %d 0\n",
		game->config->color_count,
		game->cell_count / game->config->color_count,
		game->config->grid_size);
	fprintf(game->players[1].out, "init %d %d %d 1\n",
		game->config->color_count,
		game->cell_count / game->config->color_count,
		game->config->grid_size);
}

int game_turn(game_t *game)
{
	int a, b;
	int q, r, s, value;
	char action[8];
	struct itimerval tv;
	struct sigaction act;
	float i, f;

	// Set signal handler
	act.sa_handler = handler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGALRM, &act, NULL);

	// Arm timer
	tv.it_interval = (struct timeval) { .tv_sec = 0, .tv_usec = 0 };
	f = modff(game->config->timeout, &i);
	tv.it_value.tv_sec = i;
	tv.it_value.tv_usec = f * 1000000;
	setitimer(ITIMER_REAL, &tv, NULL);

	// Take chips from bag
	a = game_take_random(game);
	if (a == -1)
		return !game->turn;
	game->chip_counts[a] += 1;
	b = game_take_random(game);
	if (b == -1)
		return !game->turn;
	game->chip_counts[b] += 1;
	fprintf(game->players[game->turn].out, "chips %d %d\n", a, b);

	// Get action from player
	if (fscanf(game->players[game->turn].in, "%7s", action) != 1)
		return !game->turn;
	
	if (strcmp(action, "rotate") == 0) {
		// Get and validate parameters
		if (fscanf(game->players[game->turn].in, "%d", &value) != 1)
			return !game->turn;
		if (value < 0 || value >= 6)
			return !game->turn;

		// Perform rotation
		fprintf(game->players[!game->turn].out, "rotate %d\n", value);
		game_rotate(game, value);
	} else if (strcmp(action, "drop") == 0) {
		// Get and validate parameters
		if (fscanf(game->players[game->turn].in, "%d %d %d %d", &q, &r, &s, &value) != 4)
			return !game->turn;
		cell_t *cell = game_get(game, q, r, s);
		if (cell == NULL || cell->chip.value != -1
			|| value < game->turn * game->config->color_count / 2
			|| value >= (game->turn + 1) * game->config->color_count / 2)
			return !game->turn;

		// Perform drop
		if (value == a)
			game->chip_counts[b] += 1;
		else
			game->chip_counts[a] += 1;
		fprintf(game->players[!game->turn].out, "drop %d %d %d %d\n", q, r, s, value);
		game_drop(game, q, r, s, value);
	} else {
		// Invalid action
		return !game->turn;
	}

	// Disarm timer
	tv.it_value = (struct timeval) { .tv_sec = 0, .tv_usec = 0 };
	setitimer(ITIMER_REAL, &tv, NULL);

	// Invert turn and check winner
	game->turn = !game->turn;
	int winner = game_winner(game);
	if (winner != -1)
		return winner * 2 / game->config->color_count;
	return -1;
}
