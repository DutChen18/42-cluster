#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	scanf("init %d %d %d %d\n", &color_count, &chip_count, &grid_size, &id);
	if (id == 1) {
		read_action(action, &q, &r, &s, &value);
	}
	while (1) {
		scanf("%d %d", &a, &b);
		printf("drop 0 0 0 %d\n", a);
		read_action(action, &q, &r, &s, &value);
	}
	return EXIT_SUCCESS;
}
