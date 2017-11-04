#include <stdlib.h>

#include <pixman.h>

#include "draw.h"

Image *
NewLinearGradient(Point p1, Point p2, GradientStop *stops, int n)
{
#ifndef NO_PIXMAN
	return drawer->lineargradient(p1, p2, stops, n);
#else
#error "NewLinearGradient not implemented"
#endif
}
