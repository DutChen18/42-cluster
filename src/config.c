#include "cluster.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void config_read(config_t *config, const char *path)
{
	FILE* file = fopen(path, "r");
	char key[16];

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

	while (fscanf(file, "%15s", key) == 1) {
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
		|| config->grid_size < 4
		|| config->bot_speed < 0
		|| config->color_count < 2
		|| config->color_count % 2 != 0
		|| config->win_length < 2
		|| config->timeout < 0
		|| config->window_width < 500
		|| config->window_height < 500
		|| (config->debug != 0 && config->debug != 1)
		|| (config->autoclose != 0 && config->autoclose != 1)) {
		fprintf(stderr, "invalid config file\n");
		exit(EXIT_FAILURE);
	}
}
