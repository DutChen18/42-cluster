#include "cluster.h"
#include <stdio.h>
#include <string.h>

void config_read(config_t *config, const char *path)
{
	FILE* file = fopen(path, "r");
	char key[16];

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
		}
	}
}
