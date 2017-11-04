#include <stdlib.h>

#include "draw.h"

void
ColorRGBA(Color c, int *r, int *g, int *b, int *a)
{
	*r = c.R;
	*g = c.G;
	*b = c.B;
	*a = c.A;
}
