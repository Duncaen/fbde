#include <stdlib.h>

#include <pixman.h>

#include "draw.h"

const int OpPixman[] = {
	[Over] = PIXMAN_OP_OVER,
	[Src] = PIXMAN_OP_SRC,
	[Add] = PIXMAN_OP_ADD,
};

static pixman_point_fixed_t
ptpixman(Point p)
{
	pixman_point_fixed_t pm;
	pm.x = p.X;
	pm.y = p.Y;
	return pm;
}

static pixman_color_t
colorpixman(Color c)
{
	pixman_color_t pc;
	pc.red = c.R;
	pc.green = c.G;
	pc.blue = c.B;
	pc.alpha = c.A;
	return pc;
}

int
imagepixmansolid(Image *img, Color c)
{
	pixman_color_t pc = colorpixman(c);
	img->_pixman_image = pixman_image_create_solid_fill(&pc);
	if (!img->_pixman_image)
		return 1;
	img->Pix = pixman_image_get_data(img->_pixman_image);
	return 0;
}

int
imagepixman(Image *img)
{
	if (img->repl)
		return 0;
	return 1;
}

void
imagepixmanfree(Image *img)
{
}

static Image *
pixmanlineargradient(Point p1, Point p2, GradientStop *stops, int n)
{
	pixman_point_fixed_t pix_p1, pix_p2;
	pixman_gradient_stop_t *pix_stops;
	Image *img;

#ifdef PIXMAN_TRACE
	printf("pixmanlineargradient:\n");
#endif
	if (!(img = malloc(sizeof (Image))))
		return 0;

	img->r = rect(-1E9, -1E9, 1E9, 1E9);
	img->clipr = img->r;
	img->chan = 0;
	img->repl = 1;
	img->fill = 0;
	img->Pix = 0;

	pix_p1 = ptpixman(p1);
	pix_p2 = ptpixman(p2);

	pix_stops = calloc(n, sizeof (pixman_gradient_stop_t));
	if (!pix_stops)
		return 0;

	for (int i = 0; i < n; i++) {
		pix_stops[i].x = pixman_double_to_fixed(stops[i].X);
		pix_stops[i].color = colorpixman(stops[i].Color);
	}

	img->_pixman_image = pixman_image_create_linear_gradient(
	    &pix_p1, &pix_p2, pix_stops, n);
}

static void
pixmandrawmask(Image *dst, Rectangle r,
    Image *src, Point sp,
    Image *mask, Point mp, Op op)
{
	pixman_image_t *ps, *pm, *pd;
	ps = pm = pd = 0;
	pd = dst->_pixman_image;
	if (src) ps = src->_pixman_image;
	if (mask) pm = mask->_pixman_image;

	int pix_op = OpPixman[op];
	if (!pix_op) pix_op = op;
	pixman_image_composite(pix_op, ps, pm, pd,
	    sp.X, sp.Y, mp.X, mp.Y, r.Min.X, r.Min.Y,
	    rectdx(r), rectdy(r));
}

static int
pixmanimagealloc(Image *img)
{
#ifdef PIXMAN_TRACE
	printf("pixmanimagealloc: repl=%d\n", img->repl);
#endif
	if (img->repl && recteq(img->r, IR)) {
#ifdef PIXMAN_TRACE
		printf("pixmanimagealloc: solid fill\n");
#endif
		pixman_color_t pc = colorpixman(*img->fill);
		img->_pixman_image = pixman_image_create_solid_fill(&pc);
	} else if (img->Pix) {
#ifdef PIXMAN_TRACE
		printf("pixmanimagealloc: alloc from data\n");
#endif
		img->_pixman_image = pixman_image_create_bits_no_clear(
		    img->chan,
		    rectdx(img->r),
		    rectdy(img->r),
		    img->Pix,
		    rectdx(img->r)*4); // TODO: calc bpp
	} else {
		img->_pixman_image = pixman_image_create_bits(
		    img->chan, rectdx(img->r), rectdy(img->r), NULL, 0);
		img->Pix = pixman_image_get_data(img->_pixman_image);
	}
	if (!img->_pixman_image)
		return 1;
	return 0;
}

static void
pixmanimagefree(Image *img)
{
	if (img->_pixman_image)
		pixman_image_unref(img->_pixman_image);
}

Drawer *drawer = &(Drawer){
	.imagealloc = pixmanimagealloc,
	.imagefree = pixmanimagefree,
	.drawmask = pixmandrawmask,
	.lineargradient = pixmanlineargradient,
};
