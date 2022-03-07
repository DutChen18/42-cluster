#ifndef CLUSTER_H
#define CLUSTER_H

#include <stddef.h>
#include "MLX42/MLX42.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600


typedef struct cell cell_t;
typedef struct game game_t;

struct cell {
	cell_t *neighbors[6];
	int value;
	int q, r, s;
	float x, y;
	float old_x, old_y;
};

struct hexagon {
	int			height;
	int			width;
	int			color;
	mlx_image_t	*img;
};

struct game {
	cell_t *cells;
	int cell_count;
	int gravity;
};

void coord_convert(float *x, float *y, int q, int r, int s);

cell_t *game_get(game_t *game, int q, int r, int s);
void game_init(game_t *game, int size);
cell_t *game_update(game_t *game, cell_t *cell);
void game_rotate(game_t *game, int gravity);
void game_drop(game_t *game, int q, int r, int s, int value);
int game_winner(game_t *game);

#endif
