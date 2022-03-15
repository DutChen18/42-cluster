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
	player->exe_name = path;
	player->is_bot = true;
	if (player->pid == 0)
	{
		close(in[STDOUT_FILENO]);
		close(out[STDIN_FILENO]);
		dup2(in[STDIN_FILENO], STDIN_FILENO);
		dup2(out[STDOUT_FILENO], STDOUT_FILENO);
		execl("/bin/sh", "/bin/sh", "-c", path, NULL);
		exit(EXIT_FAILURE);
	}
	else
	{
		close(in[STDIN_FILENO]);
		close(out[STDOUT_FILENO]);
		player->in = fdopen(out[STDIN_FILENO], "r");
		if (player->in == NULL)
			exit(EXIT_FAILURE);
		setbuf(player->in, NULL);
		player->out = fdopen(in[STDOUT_FILENO], "w");
		if (player->out == NULL)
			exit(EXIT_FAILURE);
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

static bool timed_out;

static void handler(int sig)
{
	timed_out = true;
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

static int check_winner(int winner)
{
	if (winner != -1)
		printf("Player %d wins\n", winner + 1);
	return winner;
}

static int handle_fetch(game_t *game, int turn)
{
	char target[16];

	if (fscanf(game->players[turn].in, "%15s", target) != 1) {
		if (game->config->debug)
			fprintf(stderr, "Player %d did not give fetch target\n", turn + 1);
		return 0;
	}

	if (strcmp(target, "cells") == 0) {
		fprintf(game->players[turn].out, "cell_count %d\n", game->cell_count);
		for (int i = 0; i < game->cell_count; i++) {
			cell_t *cell = &game->cells[i];
			fprintf(game->players[turn].out, "cell %d %d %d %d\n", cell->q, cell->r, cell->s, cell->chip.value);
		}
	} else if (strcmp(target, "gravity") == 0) {
		fprintf(game->players[turn].out, "gravity %d\n", game->gravity);
	} else if (strcmp(target, "walls") == 0) {
		int wall_count = 0;
		for (int i = 0; i < game->cell_count; i++)
			if (game->cells[i].wall)
				wall_count += 1;
		fprintf(game->players[turn].out, "wall_count %d\n", wall_count);
		for (int i = 0; i < game->cell_count; i++) {
			cell_t *cell = &game->cells[i];
			if (cell->wall)
				fprintf(game->players[turn].out, "wall %d %d %d\n", cell->q, cell->r, cell->s);
		}
	} else if (strcmp(target, "chips") == 0) {
		int chip_count = 0;
		for (int i = 0; i < game->cell_count; i++)
			if (game->cells[i].chip.value != -1)
				chip_count += 1;
		fprintf(game->players[turn].out, "cell_count %d\n", chip_count);
		for (int i = 0; i < game->cell_count; i++) {
			cell_t *cell = &game->cells[i];
			if (cell->chip.value != -1)
				fprintf(game->players[turn].out, "cell %d %d %d %d\n", cell->q, cell->r, cell->s, cell->chip.value);
		}
	} else {
		if (game->config->debug)
			fprintf(stderr, "Player %d invalid fetch target: \"%s\" expected \"cells\" or \"gravity\" or \"walls\" or \"chips\"\n", turn + 1, target);
		return 0;
	}

	return 1;
}

int game_start(game_t *game, const char *p1, const char *p2)
{
	int c1, c2;
	unsigned col1, col2;
	char action[8];

	if (p1 != NULL) {
		popen2(p1, &game->players[0]);
	} else {
		game->players[0].is_bot = false;
		game->players[0].exe_name = "player 1";
		col1 = 0x0000FF;
	}

	if (p2 != NULL) {
		popen2(p2, &game->players[1]);
	} else {
		game->players[1].is_bot = false;
		game->players[1].exe_name = "player 2";
		col2 = 0xFFFF00;
	}
	if (p1 != NULL) {
		arm_timer(game->config->timeout);
		fprintf(game->players[0].out, "init %d %d %d %d %f 0\n",
			game->config->color_count,
			game->cell_count / game->config->color_count,
			game->config->grid_size,
			game->config->win_length,
			game->config->timeout);
	p1_start:
		if (fscanf(game->players[0].in, "%7s", action) != 1) {
			disarm_timer();
			if (game->config->debug)
				fprintf(stderr, "Player 1 did not send color command\n");
			return check_winner(1);
		}
		if (strcmp(action, "fetch") == 0) {
			if (!handle_fetch(game, 0)) {
				disarm_timer();
				return check_winner(1);
			}
			goto p1_start;
		}
		if (strcmp(action, "color") != 0) {
			disarm_timer();
			if (game->config->debug)
				fprintf(stderr, "Player 1 sent a wrong command: \"%s\" expected \"color\"\n", action);
			return check_winner(1);
		}
		if (fscanf(game->players[0].in, "%d", &c1) != 1) {
			disarm_timer();
			if (game->config->debug)
				fprintf(stderr, "Player 1 did not send color value\n");
			return check_winner(1);
		}
		if (game->config->color_blind)
			c1 = 4; 
		switch (c1) {
		case 0: col1 = 0xFF1133; break;
		case 1: col1 = 0xFFFF00; break;
		case 3: col1 = 0x00FFFF; break;
		case 4: col1 = 0x0033FF; break;
		case 5: col1 = 0xFF00FF; break;
		default:
			disarm_timer();
			if (game->config->debug)
				fprintf(stderr, "Player 1 sent invalid color value\n");
			return check_winner(1);
		}
		disarm_timer();
	}

	if (p2 != NULL) {
		arm_timer(game->config->timeout);
		fprintf(game->players[1].out, "init %d %d %d %d %f 1\n",
			game->config->color_count,
			game->cell_count / game->config->color_count,
			game->config->grid_size,
			game->config->win_length,
			game->config->timeout);
	p2_start:
		if (fscanf(game->players[1].in, "%7s", action) != 1) {
			disarm_timer();
			if (game->config->debug)
				fprintf(stderr, "Player 2 did not send color command\n");
			return check_winner(0);
		}
		if (strcmp(action, "fetch") == 0) {
			if (!handle_fetch(game, 1)) {
				disarm_timer();
				return check_winner(0);
			}
			goto p2_start;
		}
		if (strcmp(action, "color") != 0) {
			disarm_timer();
			if (game->config->debug)
				fprintf(stderr, "Player 2 sent a wrong command: \"%s\" expected \"color\"\n", action);
			return check_winner(0);
		}
		if (fscanf(game->players[1].in, "%d", &c2) != 1) {
			disarm_timer();
			if (game->config->debug)
				fprintf(stderr, "Player 2 did not send color value\n");
			return check_winner(0);
		}
		if (game->config->color_blind)
			c2 = 1; 
		switch (c2) {
		case 0: col2 = 0xFF1133; break;
		case 1: col2 = 0xFFFF00; break;
		case 3: col2 = 0x00FFFF; break;
		case 4: col2 = 0x0033FF; break;
		case 5: col2 = 0xFF00FF; break;
		default:
			disarm_timer();
			if (game->config->debug)
				fprintf(stderr, "Player 2 sent invalid color value\n");
			return check_winner(0);
		}
		disarm_timer();
	}

	if (col1 == 0xFF00FF && col2 == 0xFF00FF)
		col2 = 0xFFFF00;
	create_chip_colors(game, col1 << 8 | 0xFF, col2 << 8 | 0xFF);
	return -1;
}

int game_preturn(game_t *game)
{
	game->chip_a = game_take_random(game);
	game->chip_b = -1;
	if (game->chip_a == -1) {
		if (game->config->debug)
			fprintf(stderr, "Player %d is out of chips\n", game->turn + 1);
		return check_winner(!game->turn);
	}
	game->chip_counts[game->chip_a] -= 1;
	game->chip_b = game_take_random(game);
	if (game->chip_b == -1) {
		if (game->config->chen_edition) {
			if (game->config->debug)
				fprintf(stderr, "Player %d is out of chips\n", game->turn + 1);
			return check_winner(!game->turn);
		}
	} else
		game->chip_counts[game->chip_b] -= 1;
	return -1;
}

int game_postturn_rotate(game_t *game, int value)
{
	if (game->players[!game->turn].is_bot)
		fprintf(game->players[!game->turn].out, "rotate %d\n", value);
	game_rotate(game, value);
	game->turn = !game->turn;

	int winner = game_winner(game);
	if (winner != -1) {
		winner = winner * 2 / game->config->color_count;
		return check_winner(winner);
	}
	return -1;
}

int game_postturn_drop(game_t *game, int q, int r, int s, int pos, int value)
{
	compute_pos(pos, game->config->grid_size, game->gravity, &q, &r, &s);
	if (value == game->chip_b)
		game->chip_counts[game->chip_a] += 1;
	else if (game->chip_b != -1)
		game->chip_counts[game->chip_b] += 1;
	if (game->players[!game->turn].is_bot)
		fprintf(game->players[!game->turn].out, "drop %d %d\n", pos, value);
	game_drop(game, q, r, s, value);
	game->turn = !game->turn;

	int winner = game_winner(game);
	if (winner != -1) {
		winner = winner * 2 / game->config->color_count;
		return check_winner(winner);
	}
	return -1;
}

int game_turn(game_t *game)
{
	int q, r, s, value, pos;
	char action[8];

	arm_timer(game->config->timeout);
	fprintf(game->players[game->turn].out, "chips %d %d\n", game->chip_a, game->chip_b);

	// Get action from player
	timed_out = false;
start:
	if (timed_out) {
		if (game->config->debug)
			fprintf(stderr, "Player %d timed out\n", game->turn + 1);
		return check_winner(!game->turn);
	}
	if (fscanf(game->players[game->turn].in, "%7s", action) != 1) {
		disarm_timer();
		if (game->config->debug)
			fprintf(stderr, "Player %d did not give action\n", game->turn + 1);
		return check_winner(!game->turn);
	}
	
	if (strcmp(action, "rotate") == 0) {
		// Get and validate parameters
		if (fscanf(game->players[game->turn].in, "%d", &value) != 1) {
			disarm_timer();
			if (game->config->debug)
				fprintf(stderr, "Player %d did not give rotate value\n", game->turn + 1);
			return check_winner(!game->turn);
		}
		if (game->chip_a == -1 || game->chip_b == -1) {
			disarm_timer();
			if (game->config->debug)
				fprintf(stderr, "Player %d tried to rotate with insufficient chips\n", game->turn + 1);
			return check_winner(!game->turn);
		}
		if (value < 0 || value >= 6) {
			disarm_timer();
			if (game->config->debug)
				fprintf(stderr, "Player %d sent invalid rotation value: %d\n", game->turn + 1, value);
			return check_winner(!game->turn);
		}

		// Perform rotation
		int winner = game_postturn_rotate(game, value);
		disarm_timer();
		return winner;
	} else if (strcmp(action, "drop") == 0) {
		// Get and validate parameters
		if (fscanf(game->players[game->turn].in, "%d %d", &pos, &value) != 2) {
			disarm_timer();
			if (game->config->debug)
				fprintf(stderr, "Player %d did not give drop value\n", game->turn + 1);
			return check_winner(!game->turn);
		}
		compute_pos(pos, game->config->grid_size, game->gravity, &q, &r, &s);
		cell_t *cell = game_get(game, q, r, s);
		if (cell == NULL) {
			disarm_timer();
			if (game->config->debug)
				fprintf(stderr, "Player %d tried to place a chip at an invalid location: %d\n", game->turn + 1, pos);
			return check_winner(!game->turn);
		}
		if (cell->chip.value != -1) {
			disarm_timer();
			if (game->config->debug)
				fprintf(stderr, "Player %d tried to place a chip on top of another chip\n", game->turn + 1);
			return check_winner(!game->turn);
		}
		if ((value != game->chip_a && value != game->chip_b) || value == -1) {
			disarm_timer();
			if (game->config->debug)
				fprintf(stderr, "Player %d tried to place an invalid chip: %d\n", game->turn + 1, value);
			return check_winner(!game->turn);
		}

		// Perform drop
		int winner = game_postturn_drop(game, q, r, s, pos, value);
		disarm_timer();
		return winner;
	} else if (strcmp(action, "fetch") == 0) {
		if (!handle_fetch(game, game->turn)) {
			disarm_timer();
			return check_winner(!game->turn);
		}
		goto start;
	} else {
		// Invalid action
		disarm_timer();
		if (game->config->debug)
			fprintf(stderr, "Player %d invalid action: \"%s\" expected \"rotate\" or \"drop\" or \"fetch\"\n", game->turn + 1, action);
		return check_winner(!game->turn);
	}
}
