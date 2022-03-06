#ifndef CLUSTER_H
#define CLUSTER_H

#include <stddef.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600


typedef struct cell cell_t;
typedef struct game game_t;

typedef struct cell {
	cell_t *neighbors[6];
	int value;
	int q, r, s;
}		cell_t;

typedef struct game {
	cell_t *cells;
	int cell_count;
}		game_t;

void game_init(game_t *game, int size);

#endif
