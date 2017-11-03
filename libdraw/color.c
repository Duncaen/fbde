#include <stdlib.h>

#include <pixman.h>

#include "draw.h"

void
ColorRGBA(Color c, int *r, int *g, int *b, int *a)
{
	*r = c.R;
	*g = c.G;
	*b = c.B;
	*a = c.A;
}

pixman_color_t
ColorPixman(Color c)
{
	pixman_color_t pc;
	pc.red = c.R;
	pc.green = c.G;
	pc.blue = c.B;
	pc.alpha = c.A;
	return pc;
}
