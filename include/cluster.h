#ifndef CLUSTER_H
#define CLUSTER_H

#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include "MLX42/MLX42.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

typedef struct cell cell_t;
typedef struct game game_t;
typedef struct player player_t;

struct cell {
	cell_t	*neighbors[6];
	int		value;
	int		q, r, s;
	float	x, y;
	float	old_x, old_y;
};

struct hexagon {
	int			height;
	int			width;
	int			color;
	mlx_image_t	*img;
};

struct game {
	int		cell_count;
	int		color_count;
	int		grid_size;
	cell_t	*cells;
	int		*colors;
	int		*chip_counts;
	int		gravity;
	int		turn;
};

struct player {
	pid_t	pid;
	FILE	*in;
	FILE	*out;
};

void coord_convert(float *x, float *y, int q, int r, int s);

cell_t *game_get(game_t *game, int q, int r, int s);
void game_init(game_t *game, int size, int color_count);
cell_t *game_update(game_t *game, cell_t *cell);
void game_rotate(game_t *game, int gravity);
void game_drop(game_t *game, int q, int r, int s, int value);
int game_winner(game_t *game);

#endif
