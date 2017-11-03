#include <stdlib.h>

#include <pixman.h>

#include "draw.h"

const int OpPixman[] = {
	[Over] = PIXMAN_OP_OVER,
	[Src] = PIXMAN_OP_SRC,
};
