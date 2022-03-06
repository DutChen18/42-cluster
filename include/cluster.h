#ifndef CLUSTER_H
#define CLUSTER_H

#include <stddef.h>

struct cell {
	struct cell *neighbors[6];
};

struct game {
	struct cell *cells;
	size_t cell_count;
};

#endif
