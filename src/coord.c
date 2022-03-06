#include "cluster.h"

void coord_convert(float *x, float *y, int q, int r, int s)
{
	(void) s;
	*x = (int) (q + (r - (r & 1)) / 2);
	*y = r * 0.75f;
}
