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

static void compute_pos(int pos, int size, int gravity, int *q, int *r, int *s)
{
	size -= 1;
	*q = pos;
	if (pos < 0) {
		*r = -size - pos;
		*s = size;
	} else {
		*r = -size;
		*s = size - pos;
	}
	while (gravity > -3)
	{
		int tmp = *q;
		*q = -*r;
		*r = -*s;
		*s = -tmp;
		gravity -= 1;
	}
}

static void handler(int sig)
{
	(void) sig;
}

static void arm_timer(float timeout)
{
	struct itimerval tv;
	struct sigaction act;
	float i, f;

	act.sa_handler = handler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGALRM, &act, NULL);

	tv.it_interval = (struct timeval) { .tv_sec = 0, .tv_usec = 0 };
	f = modff(timeout, &i);
	tv.it_value.tv_sec = i;
	tv.it_value.tv_usec = f * 1000000;
	setitimer(ITIMER_REAL, &tv, NULL);

}

static void disarm_timer(void)
{
	struct itimerval tv;

	tv.it_interval = (struct timeval) { .tv_sec = 0, .tv_usec = 0 };
	tv.it_value = (struct timeval) { .tv_sec = 0, .tv_usec = 0 };
	setitimer(ITIMER_REAL, &tv, NULL);
}

int game_start(game_t *game, const char *p1, const char *p2)
{
	int c1, c2;
	unsigned col1, col2;
	char action[8];

	arm_timer(game->config->timeout);
	popen2(p1, &game->players[0]);
	fprintf(game->players[0].out, "init %d %d %d %f 0\n",
		game->config->color_count,
		game->cell_count / game->config->color_count,
		game->config->grid_size,
		game->config->timeout);
	if (fscanf(game->players[0].in, "%7s", action) != 1) {
		if (game->config->debug)
			fprintf(stderr, "Player 1 timed out sending color command\n");
		return 1;
	}
	if (strcmp(action, "color") != 0) {
		if (game->config->debug)
			fprintf(stderr, "Player 1 send a wrong command: \"%s\" expected \"color\"\n", action);
		return 1;
	}
	if (fscanf(game->players[0].in, "%d", &c1) != 1) {
		if (game->config->debug)
			fprintf(stderr, "Player 1 timed out giving color value\n");
		return 1;
	}
	switch (c1) {
	case 0: col1 = 0xFF0000; break;
	case 1: col1 = 0xFFFF00; break;
	case 3: col1 = 0x00FFFF; break;
	case 4: col1 = 0x0000FF; break;
	case 5: col1 = 0xFF00FF; break;
	default: return 1;
	}
	disarm_timer();
	
	arm_timer(game->config->timeout);
	popen2(p2, &game->players[1]);
	fprintf(game->players[1].out, "init %d %d %d %f 1\n",
		game->config->color_count,
		game->cell_count / game->config->color_count,
		game->config->grid_size,
		game->config->timeout);
	if (fscanf(game->players[1].in, "%7s", action) != 1) {
		if (game->config->debug)
			fprintf(stderr, "Player 2 timed out sending color command\n");
		return 0;
	}
	if (strcmp(action, "color") != 0) {
		if (game->config->debug)
			fprintf(stderr, "Player 2 send a wrong command: \"%s\" expected \"color\"\n", action);
		return 0;
	}
	if (fscanf(game->players[1].in, "%d", &c2) != 1) {
		if (game->config->debug)
			fprintf(stderr, "Player 2 timed out giving color value\n");
		return 0;
	}
	switch (c2) {
	case 0: col2 = 0xFF0000; break;
	case 1: col2 = 0xFFFF00; break;
	case 3: col2 = 0x00FFFF; break;
	case 4: col2 = 0x0000FF; break;
	case 5: col2 = 0xFF00FF; break;
	default: return 0;
	}
	disarm_timer();

	if (col1 == 0xFF00FF && col2 == 0xFF00FF)
		col2 = 0xFFFF00;
	create_chip_colors(game, col1 << 8 | 0xFF, col2 << 8 | 0xFF);
	return -1;
}

void game_preturn(game_t *game)
{
	if (game->config->debug) {
		printf("chips");
		for (int i = 0; i < game->config->color_count; i++)
			printf(" %d", game->chip_counts[i]);
		printf("\n");
	}
	game->chip_a = game_take_random(game);
	game->chip_b = -1;
	if (game->chip_a == -1)
		return;
	game->chip_counts[game->chip_a] -= 1;
	game->chip_b = game_take_random(game);
	if (game->chip_b == -1)
		return;
	game->chip_counts[game->chip_b] -= 1;
}

int game_turn(game_t *game)
{
	int q, r, s, value, pos;
	char action[8];

	arm_timer(game->config->timeout);
	if (game->chip_a == -1 || game->chip_b == -1) {
		if (game->config->debug)
			fprintf(stderr, "Player %d out of chips: chip 1: %d, chip 2: %d\n", game->turn + 1, game->chip_a, game->chip_b);
		return !game->turn;
	}
	fprintf(game->players[game->turn].out, "chips %d %d\n", game->chip_a + 1, game->chip_b + 1);

	// Get action from player
	if (fscanf(game->players[game->turn].in, "%7s", action) != 1) {
		if (game->config->debug)
			fprintf(stderr, "Player %d timed out giving action\n", game->turn + 1);
		return !game->turn;
	}
	
	if (strcmp(action, "rotate") == 0) {
		// Get and validate parameters
		if (fscanf(game->players[game->turn].in, "%d", &value) != 1) {
			if (game->config->debug)
				fprintf(stderr, "Player %d timed out giving rotate value\n", game->turn + 1);
			return !game->turn;
		}
		if (value < 0 || value >= 6) {
			if (game->config->debug)
				fprintf(stderr, "Player %d rotate error: %d\n", game->turn + 1, value);
			return !game->turn;
		}

		// Perform rotation
		fprintf(game->players[!game->turn].out, "rotate %d\n", value);
		game_rotate(game, value);
	} else if (strcmp(action, "drop") == 0) {
		// Get and validate parameters
		if (fscanf(game->players[game->turn].in, "%d %d", &pos, &value) != 2) {
			if (game->config->debug)
				fprintf(stderr, "Player %d timed out giving drop value\n", game->turn + 1);
			return !game->turn;
		}
		compute_pos(pos, game->config->grid_size, game->gravity, &q, &r, &s);
		cell_t *cell = game_get(game, q, r, s);
		if (cell == NULL || cell->chip.value != -1
			|| value < game->turn * game->config->color_count / 2
			|| value >= (game->turn + 1) * game->config->color_count / 2) {
			if (game->config->debug)
				fprintf(stderr, "Player %d tried to place a cell in a wrong location\n", game->turn + 1);
			return !game->turn;
		}

		// Perform drop
		if (value == game->chip_a)
			game->chip_counts[game->chip_b] += 1;
		else
			game->chip_counts[game->chip_a] += 1;
		fprintf(game->players[!game->turn].out, "drop %d %d\n", pos, value);
		game_drop(game, q, r, s, value);
	} else {
		// Invalid action
		if (game->config->debug)
			fprintf(stderr, "Player %d invalid action: \"%s\" expected \"rotate\" or \"drop\"\n", game->turn + 1, action);
		return !game->turn;
	}
	disarm_timer();

	// Invert turn and check winner
	game->turn = !game->turn;
	int winner = game_winner(game);
	if (winner != -1)
		return winner * 2 / game->config->color_count;
	return -1;
}
