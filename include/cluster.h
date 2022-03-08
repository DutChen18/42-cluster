#ifndef CLUSTER_H
#define CLUSTER_H

// Included libraries
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include "MLX42/MLX42.h"

//Some basic definitons to be used by the game (have to make a config later).
#define WINDOW_WIDTH 1080
#define WINDOW_HEIGHT 1080
#define GRID_BORDER_SIZE 6 //best a even number
#define SIZE 20 //max is 55 if we fix 1 image for the grid, otherwise devide by 1.7
#define WIN_LENGTH 6

// Numpad values (unused).
# define NUMPAD_7_KEY 89
# define NUMPAD_8_KEY 91
# define NUMPAD_9_KEY 92
# define NUMPAD_4_KEY 86
# define NUMPAD_5_KEY 87
# define NUMPAD_6_KEY 88

// Key values (also unused).
# define UP_ARROW_KEY 126
# define DOWN_ARROW_KEY 125
# define LEFT_ARROW_KEY 123
# define RIGHT_ARROW_KEY 124
# define ESC_KEY 53

// Setting all structures names from "*" to "*_t".
typedef struct cell cell_t;
typedef struct game game_t;
typedef struct grid grid_t;
typedef struct hexagon hexagon_t;
typedef struct player player_t;
typedef struct cluster cluster_t;

// The structure used for making game logic cells. Cells are used to place and connect the logic of the grid. Quatation needed.
struct cell {
	cell_t		*neighbors[6];
	int			value;
	int			q, r, s;
	float		x, y;
	float		old_x, old_y;
	int			tile_instance;
	mlx_image_t	*image;
	bool		placed;
};

// Used to store Hexagon properties
struct hexagon {
	int			height;
	int			width;
	int			color;
	mlx_image_t	*img;
};

// The data for the grid
struct grid {
	int			height;
	int			width;
	mlx_image_t	*grid;
	hexagon_t	one_cell;
};

// All game logic is stored in here
struct game {
	int			cell_count;
	int			rings;
	int			cell_diagonal;
	int			cell_height;
	int			color_count;
	int			grid_size;
	cell_t		*cells;
	int			*colors;
	hexagon_t	*hexa_tiles;
	int			*chip_counts;
	int			gravity;
	mlx_image_t *bg_gradients[6];
	int			turn;
};

// Structure used for bot implementation
struct player {
	pid_t	pid;
	FILE	*in;
	FILE	*out;
};

// All data is stored in this structure
struct cluster
{
	mlx_t			*mlx;
	game_t			game;
	grid_t			grid;
	mlx_key_data_t	move;
	bool			moving;
	double			time;
	player_t		players[2];
	int				winner;
};

// color functions
int	create_color(int r, int g, int b, int t);

// coordinate functions
void coord_convert(float *x, float *y, int q, int r, int s);

// width and height functions
int	get_width_from_height(int height);
int	get_height_from_width(int width);

// Game logic functions
cell_t *game_get(game_t *game, int q, int r, int s);
void game_init(mlx_t *mlx, game_t *game, int size, int color_count);
cell_t *game_update(game_t *game, cell_t *cell);
void game_rotate(game_t *game, int gravity);
void game_drop(game_t *game, int q, int r, int s, int value);
int game_winner(game_t *game);
void game_start(game_t *game, player_t *players);
int game_turn(game_t *game, player_t *players);
void popen2(const char *path, player_t *player);

#endif // CLUSTER_H
