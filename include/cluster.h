#ifndef CLUSTER_H
#define CLUSTER_H

#include <stddef.h>

/* -=-=- Define -=-=- */

//colors
# define COLOR_RED 0xFF000088
# define COLOR_GREEN 0x00FF0088

//window
# define WINDOW_WIDTH 800
# define WINDOW_HEIGHT 600

typedef struct s_cell {
	struct cell *neighbors[6];
}				t_cell;

struct s_game {
	struct cell*	cells;
	size_t			cell_count;
}					t_game;

#endif
