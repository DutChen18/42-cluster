#ifndef CLUSTER_H
#define CLUSTER_H

#include <stddef.h>

typedef struct cell cell_t;
typedef struct game game_t;

struct cell {
	cell_t *neighbors[6];
	int value;
	int q, r, s;
};

struct game {
	cell_t *cells;
	int cell_count;
};

void game_init(game_t *game, int size);

#endif
