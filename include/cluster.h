#ifndef CLUSTER_H
#define CLUSTER_H
#define _POSIX_C_SOURCE 1

// Included libraries
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include "MLX42/MLX42.h"

//instance levels
# define DISMISS -1
# define BACKROUND 0
# define GRADIENTS 1
# define GRID 20
# define GRID_LINE 30
# define HEXAGON 40

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
typedef struct gui gui_t;
typedef struct bag_count bag_count_t;

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
	bool	wall;
	int		q, r, s;
	float	x, y;
	bool	is_winning;
};

// Used to store Hexagon properties
struct hexagon {
	int			height;
	int			width;
	int			background_color;
	int			border_color;
	mlx_image_t	*img;
};

// The data for the grid
struct grid {
	int			height;
	int			width;
	mlx_image_t	*grid;
	hexagon_t	one_cell;
	int			background_color;
	int			border_color;
};

struct config {
	int			use_mlx;
	int			grid_size;
	int			color_count;
	float		bot_speed;
	int			win_length;
	float		timeout;
	int			window_width;
	int			window_height;
	int			debug;
	int			autoclose;
	unsigned	bg_color;
	unsigned	bg_gradient_color;
	float		wall_chance;
	int			wall_seed;
	int			chen_edition;
	unsigned	cell_bg_color;
	unsigned	cell_border_color;
	unsigned	win_bg_color;
	unsigned	win_border_color;
	int			color_blind;
};

// Structure used for bot implementation
struct player {
	pid_t		pid;
	FILE		*in;
	FILE		*out;
	const char	*exe_name;
	bool		is_bot;
};

struct gui {
	int			x;
	int			y;
	int			layer;
	hexagon_t	*back_cell;
	hexagon_t	*colors;
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
	int			chip_a, chip_b;
};

struct bag_count {
	mlx_image_t	*text;
	int			background;
	int			x;
	int			y;
};

struct visuals {
	mlx_t		*mlx;
	int			cell_diagonal;
	int			cell_height;
	hexagon_t	*hexa_tiles;
	mlx_image_t	*bg_gradients[6];
	grid_t		grid;
	gui_t		gui[4];
	bool		skip_next;
	mlx_image_t	*winner_str;
	bag_count_t	bag_counts[2];
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
	bool			left_state;
	bool			right_state;
};

int		get_border_size(int height);
void	hexagon_border_init(visuals_t *visuals, hexagon_t *obj, int width, int height, int background_color, int border_color);
void	place_border(config_t *config, visuals_t *visuals, cell_t *cell, hexagon_t *texture, int color);
void	hexagon_init(mlx_t *mlx, hexagon_t *obj, int width, int height, int color);
void	place_hexagon(config_t *config, visuals_t *visuals, cell_t *cell);

// color functions
int	create_color(int r, int g, int b, int t);
void create_chip_colors(game_t *game, unsigned base_color_p1, unsigned base_color_p2);

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

int game_preturn(game_t *game);
int game_turn(game_t *game);
int game_postturn_rotate(game_t *game, int value);
int game_postturn_drop(game_t *game, int q, int r, int s, int pos, int value);

void grid_init(visuals_t *visuals, game_t *game);
void set_bg_gradients(config_t *config, mlx_t* mlx, mlx_image_t **bg_gradients);
void visuals_init(visuals_t *visuals, mlx_t *mlx, game_t *game);

void config_read(config_t *config, const char *path, char *exe_name);

#endif // CLUSTER_H
