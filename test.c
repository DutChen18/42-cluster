#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

void read_action(char *action, int *pos, int *value)
{
	scanf("%255s", action);
	if (strcmp(action, "rotate") == 0) {
		scanf("%d", value);
	} else if (strcmp(action, "drop") == 0) {
		scanf("%d %d", pos, value);
	}
}

int main(void)
{
	int color_count, chip_count, grid_size, id;
	int pos, value;
	char action[256];
	float timeout;
	int a, b;
	setbuf(stdout, NULL);
	setbuf(stdin, NULL);
	setbuf(stderr, NULL);
	scanf(" init %d %d %d %f %d", &color_count, &chip_count, &grid_size, &timeout, &id);
	printf("color %d\n", arc4random() % 6);
	if (id == 1) {
		read_action(action, &pos, &value);
	}
	while (1) {
		scanf(" chips %d %d", &a, &b);
		if (arc4random() % 2 == 0)
			printf("rotate %d\n", arc4random() % 6);
		else
			printf("drop %d %d\n", (int) (arc4random() % (grid_size * 2 - 1)) - grid_size + 1, a);
		read_action(action, &pos, &value);
	}
	return EXIT_SUCCESS;
}
