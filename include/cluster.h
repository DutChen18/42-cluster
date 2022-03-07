#ifndef CLUSTER_H
#define CLUSTER_H

#include <stddef.h>
#include "MLX42/MLX42.h"

#define WINDOW_WIDTH 2560 * 2
#define WINDOW_HEIGHT 1440 * 2
#define GRID_BORDER_SIZE 8 //best a even number
#define SIZE 8 //max is 55 if we fix 1 image for the grid, otherwise devide by 1.7

typedef struct cell cell_t;
typedef struct game game_t;
typedef struct grid grid_t;
typedef struct hexagon hexagon_t;

struct cell {
	cell_t *neighbors[6];
	int value;
	int q, r, s;
	float x, y;
	float old_x, old_y;
};

struct grid {
	int			height;
	int			width;
	mlx_image_t	*grid;
	hexagon_t	*one_cell;
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
	int rings;
	int cell_diagonal;
	int cell_height;
};

void coord_convert(float *x, float *y, int q, int r, int s);

int	get_width_from_height(int height);
int	get_height_from_width(int width);

cell_t *game_get(game_t *game, int q, int r, int s);
void game_init(game_t *game, int size);
cell_t *game_update(game_t *game, cell_t *cell);
void game_rotate(game_t *game, int gravity);
void game_drop(game_t *game, int q, int r, int s, int value);

#endif
