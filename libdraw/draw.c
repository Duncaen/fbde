#include <stdlib.h>
#include <assert.h>

#include <pixman.h>

#include "draw.h"

void
clip(Image *dst, Rectangle *r,
    Image *src, Point *sp, Image *mask, Point *mp)
{
	Point orig = r->Min;
	*r = RectIntersect(*r, dst->Rect);
	*r = RectIntersect(*r, RectAdd(src->Rect, PointSub(orig, *sp)));
	if (mask) {
		*r = RectIntersect(*r, RectAdd(mask->Rect, PointSub(orig, *mp)));
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
DrawMask(Image *dst, Rectangle r,
    Image *src, Point sp,
    Image *mask, Point mp, Op op)
{
	assert(dst && src);
	clip(dst, &r, src, &sp, mask, &mp);

	pixman_image_t *ps, *pm, *pd;
	ps = pm = pd = 0;
	pd = dst->_pix;
	if (src) ps = src->_pix;
	if (mask) pm = mask->_pix;

	int pix_op = OpPixman[op];
	if (!pix_op) pix_op = op;
	pixman_image_composite(pix_op, ps, pm, pd,
	    sp.X, sp.Y, mp.X, mp.Y, r.Min.X, r.Min.Y,
	    RectDx(r), RectDy(r));
}

void
Draw(Image *dst, Rectangle r, Image *src, Point sp, Op op)
{
	DrawMask(dst, r, src, sp, 0, ZP, op);
}
