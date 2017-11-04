#include <stdlib.h>
#include <assert.h>

#include "draw.h"

static void
clip(Image *dst, Rectangle *r,
    Image *src, Point *sp, Image *mask, Point *mp)
{
	Point orig = r->Min;
	*r = rectintersect(*r, dst->r);
	*r = rectintersect(*r, rectadd(src->r, ptsub(orig, *sp)));
	if (mask) {
		*r = rectintersect(*r, rectadd(mask->r, ptsub(orig, *mp)));
	}
	int dx, dy;
	dx = r->Min.X - orig.X;
	dy = r->Min.Y - orig.Y;
	if (dx == 0 && dy == 0)
		return;
	sp->X += dx;
	sp->Y += dy;
	if (mask) {
		mp->X += dx;
		mp->Y += dy;
	}
}

void
drawmask(Image *dst, Rectangle r,
    Image *src, Point sp,
    Image *mask, Point mp, Op op)
{
	assert(dst && src);
	clip(dst, &r, src, &sp, mask, &mp);
	drawer->drawmask(dst, r, src, sp, mask, mp, op);
}

void
draw(Image *dst, Rectangle r, Image *src, Point sp, Op op)
{
	drawmask(dst, r, src, sp, 0, ZP, op);
}
