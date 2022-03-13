#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static unsigned long mix(unsigned long a, unsigned long b, unsigned long c)
{
	a = (a - b - c) ^ (c >> 13);
	b = (b - c - a) ^ (a << 8);
	c = (c - a - b) ^ (b >> 13);
	a = (a - b - c) ^ (c >> 12);
	b = (b - c - a) ^ (a << 16);
	c = (c - a - b) ^ (b >> 5);
	a = (a - b - c) ^ (c >> 3);
	b = (b - c - a) ^ (a << 10);
	c = (c - a - b) ^ (b >> 15);
	return c;
}

static int next(int max)
{
	return (rand() / 256) % max;
}

static void read_action(char *action, int *pos, int *value)
{
	scanf("%255s", action);
	if (strcmp(action, "rotate") == 0) {
		scanf("%d", value);
	} else if (strcmp(action, "drop") == 0) {
		scanf("%d %d", pos, value);
	}
}

static int read_gravity(void)
{
	int gravity;
	printf("fetch gravity\n");
	scanf(" gravity %d", &gravity);
	return gravity;
}

static void compute_pos(int pos, int size, int gravity, int *q, int *r, int *s)
{
	size -= 1;
	*q = pos;
	if (pos < 0) {
		*r = -size - pos;
		*s = size;
	} else {
		*r = -size;
		*s = size - pos;
	}
	while (gravity > -3)
	{
		int tmp = *q;
		*q = -*r;
		*r = -*s;
		*s = -tmp;
		gravity -= 1;
	}
}

static int read_emptiness(int q, int r, int s)
{
	int q2, r2, s2, value;
	int count, result = 1;
	printf("fetch chips\n");
	scanf(" cell_count %d", &count);
	for (int i = 0; i < count; i++) {
		scanf(" cell %d %d %d %d", &q2, &r2, &s2, &value);
		if (q == q2 && r == r2 && s == s2)
			result = 0;
	}
	return result;
}

int main(void)
{
	int color_count, chip_count, grid_size, win_length, id;
	int pos, value;
	char action[256];
	float timeout;
	int a, b;
	int c;
	int q, r, s;
	setbuf(stdout, NULL);
	setbuf(stdin, NULL);
	setbuf(stderr, NULL);
	scanf(" init %d %d %d %d %f %d", &color_count, &chip_count, &grid_size, &win_length, &timeout, &id);
	srand(mix(clock(), time(NULL), getpid()));
	c = next(5);
	printf("color %d\n", c >= 2 ? c + 1 : c);
	if (id == 1) {
		read_action(action, &pos, &value);
	}
	while (1) {
		scanf(" chips %d %d", &a, &b);
		if (next(3) == 0 && b != -1) {
			value = next(5);
			printf("rotate %d\n", value >= read_gravity() ? value + 1 : value);
		} else {
			while (1) {
				pos = next(grid_size * 2 - 1) - grid_size + 1;
				compute_pos(pos, grid_size, read_gravity(), &q, &r, &s);
				if (read_emptiness(q, r, s)) {
					printf("drop %d %d\n", pos, a);
					break;
				}
			}
		}
		read_action(action, &pos, &value);
	}
	return EXIT_SUCCESS;
}
