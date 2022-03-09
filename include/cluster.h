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
typedef struct chip chip_t;
typedef struct cell cell_t;
typedef struct game game_t;
typedef struct grid grid_t;
typedef struct hexagon hexagon_t;
typedef struct player player_t;
typedef struct cluster cluster_t;
typedef struct config config_t;
typedef struct visuals visuals_t;

// Data that gets copied when a chip moves to another cell.
struct chip {
	int		value;
	float	x, y;
	int		tile_index;
	bool	placed;
};

// The structure used for making game logic cells.
// Cells are used to place and connect the logic of the grid.
struct cell {
	cell_t	*neighbors[6];
	chip_t	chip;
	int		q, r, s;
	float	x, y;
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

struct config {
	int		use_mlx;
	int		grid_size;
	int		color_count;
	float	bot_speed;
	int		win_length;
	float	timeout;
};

// Structure used for bot implementation
struct player {
	pid_t	pid;
	FILE	*in;
	FILE	*out;
};

// All game logic is stored in here
struct game {
	config_t	*config;
	player_t	players[2];
	int			cell_count;
	cell_t		*cells;
	int			*colors;
	int			*chip_counts;
	int			gravity;
	int			turn;
};

struct visuals {
	mlx_t		*mlx;
	int			cell_diagonal;
	int			cell_height;
	hexagon_t	*hexa_tiles;
	mlx_image_t	*bg_gradients[6];
	grid_t		grid;
	bool		skip_next;
};

// All data is stored in this structure
struct cluster {
	game_t			game;
	visuals_t		visuals;
	mlx_key_data_t	move;
	bool			moving;
	double			time;
	int				winner;
	bool			needs_move;
};

// color functions
int	create_color(int r, int g, int b, int t);

// coordinate functions
void coord_convert(float *x, float *y, int q, int r, int s);

// width and height functions
int	get_width_from_height(int height);
int	get_height_from_width(int width);

// Game logic functions
void game_init(game_t *game, config_t *config);
cell_t *game_get(game_t *game, int q, int r, int s);
cell_t *game_update(game_t *game, cell_t *cell);
void game_rotate(game_t *game, int gravity);
void game_drop(game_t *game, int q, int r, int s, int value);
int game_winner(game_t *game);
int game_start(game_t *game, const char *p1, const char *p2);
int game_turn(game_t *game);

void grid_init(visuals_t *visuals, game_t *game);
void set_bg_gradients(mlx_t* mlx, mlx_image_t **bg_gradients);
void visuals_init(visuals_t *visuals, mlx_t *mlx, game_t *game);

void config_read(config_t *config, const char *path);

#endif // CLUSTER_H
