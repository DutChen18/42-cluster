#include "cluster.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void config_read(config_t *config, const char *path)
{
	FILE* file = fopen(path, "r");
	char key[32];

	config->use_mlx = -1;
	config->grid_size = -1;
	config->bot_speed = -1;
	config->color_count = -1;
	config->win_length = -1;
	config->timeout = -1;
	config->window_width = -1;
	config->window_height = -1;
	config->debug = -1;
	config->autoclose = -1;
	config->bg_color = 0;
	config->bg_gradient_color = 0;
	config->wall_chance = -1;
	config->wall_seed = -1;
	config->chen_edition = -1;
	config->cell_bg_color = 0;
	config->cell_border_color = 0;
	config->win_bg_color = 0;
	config->win_border_color = 0;

	while (fscanf(file, "%31s", key) == 1) {
		if (strcmp(key, "use_mlx") == 0) {
			fscanf(file, "%d", &config->use_mlx);
		} else if (strcmp(key, "grid_size") == 0) {
			fscanf(file, "%d", &config->grid_size);
		} else if (strcmp(key, "bot_speed") == 0) {
			fscanf(file, "%f", &config->bot_speed);
		} else if (strcmp(key, "color_count") == 0) {
			fscanf(file, "%d", &config->color_count);
			config->color_count *= 2;
		} else if (strcmp(key, "win_length") == 0) {
			fscanf(file, "%d", &config->win_length);
		} else if (strcmp(key, "timeout") == 0) {
			fscanf(file, "%f", &config->timeout);
		} else if (strcmp(key, "window_width") == 0) {
			fscanf(file, "%d", &config->window_width);
		} else if (strcmp(key, "window_height") == 0) {
			fscanf(file, "%d", &config->window_height);
		} else if (strcmp(key, "debug") == 0) {
			fscanf(file, "%d", &config->debug);
		} else if (strcmp(key, "autoclose") == 0) {
			fscanf(file, "%d", &config->autoclose);
		} else if (strcmp(key, "bg_color") == 0) {
			fscanf(file, "%X", &config->bg_color);
		} else if (strcmp(key, "bg_gradient_color") == 0) {
			fscanf(file, "%X", &config->bg_gradient_color);
		} else if (strcmp(key, "wall_chance") == 0) {
			fscanf(file, "%f", &config->wall_chance);
		} else if (strcmp(key, "wall_seed") == 0) {
			fscanf(file, "%d", &config->wall_seed);
		} else if (strcmp(key, "chen_edition") == 0) {
			fscanf(file, "%d", &config->chen_edition);
		} else if (strcmp(key, "cell_bg_color") == 0) {
			fscanf(file, "%X", &config->cell_bg_color);
		} else if (strcmp(key, "cell_border_color") == 0) {
			fscanf(file, "%X", &config->cell_border_color);
		} else if (strcmp(key, "win_bg_color") == 0) {
			fscanf(file, "%X", &config->win_bg_color);
		} else if (strcmp(key, "win_border_color") == 0) {
			fscanf(file, "%X", &config->win_border_color);
		} else if (strcmp(key, "#") == 0) {
			while (1) {
				int c = fgetc(file);
				if (c == '\n' || c == EOF)
					break;
			}
		}
	}

	if (config->color_count / 2 > config->grid_size)
		config->color_count = config->grid_size * 2;

	if ((config->use_mlx != 0 && config->use_mlx != 1)
		|| config->bot_speed < 0
		|| config->grid_size < 4
		|| config->color_count < 2
		|| config->color_count % 2 != 0
		|| config->win_length < 2
		|| config->timeout < 0
		|| config->window_width < 500
		|| config->window_height < 500
		|| (config->debug != 0 && config->debug != 1)
		|| (config->autoclose != 0 && config->autoclose != 1)
		|| config->wall_chance < 0
		|| config->wall_seed < 0
		|| (config->chen_edition != 0 && config->chen_edition != 1)) {
		fprintf(stderr, "invalid config file\n");
		exit(EXIT_FAILURE);
	}

	config->cell_bg_color = config->cell_bg_color << 8 | 0xff;
	config->cell_border_color = config->cell_border_color << 8 | 0xff;
	config->win_bg_color = config->win_bg_color << 8 | 0xff;
	config->win_border_color = config->win_border_color << 8 | 0xff;
}

		// || config->grid_size < 4
