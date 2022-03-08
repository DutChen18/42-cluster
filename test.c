#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void read_action(char *action, int *q, int *r, int *s, int *value)
{
	scanf("%255s", action);
	if (strcmp(action, "rotate") == 0) {
		scanf("%d", value);
	} else if (strcmp(action, "drop") == 0) {
		scanf("%d %d %d %d", q, r, s, value);
	}
}

int main(void)
{
	int color_count, chip_count, grid_size, id;
	int q, r, s, value;
	char action[256];
	int a, b;
	setbuf(stdout, NULL);
	setbuf(stdin, NULL);
	setbuf(stderr, NULL);
	scanf(" init %d %d %d %d", &color_count, &chip_count, &grid_size, &id);
	if (id == 1) {
		read_action(action, &q, &r, &s, &value);
	}
	srand(time(NULL));
	while (1) {
		scanf(" chips %d %d", &a, &b);
		if (rand() % 2 == 0)
			printf("rotate %d\n", rand() % 6);
		else
			printf("drop 0 0 0 %d\n", a);
		read_action(action, &q, &r, &s, &value);
	}
	return EXIT_SUCCESS;
}
