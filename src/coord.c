#include "cluster.h"
#include <math.h>

void coord_convert(float *x, float *y, int q, int r, int s)
{
	(void) s;
	*x = q * sqrt(3) / 2;
	*y = (int) (r + (q - (q & 1)) / 2) + (q & 1) * 0.5f;
}
