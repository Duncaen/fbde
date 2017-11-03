#include <stdlib.h>

#include <pixman.h>

#include "draw.h"

static void
_image_init_pixman(Image *img)
{
}

Image *
allocImage()
{
	Image *img;
	if (!(img = malloc(sizeof (Image))))
		return 0;
	img->Rect = ZR;
	return img;
}

static Image *
pixmanImage(Image *img)
{
	img->_pix = pixman_image_create_bits(
	    img->_pix_fmt, RectDx(img->Rect), RectDy(img->Rect), NULL, 0);
	return img;
}

void *
ImageMem(Image *img)
{
	return pixman_image_get_data(img->_pix);
}

Image *
NewAlpha(Rectangle r)
{
	Image *img = allocImage();
	img->Rect = r;
	img->_pix_fmt = PIXMAN_a8;
	return pixmanImage(img);
}

Image *
NewAlpha16(Rectangle r)
{
	Image *img = allocImage();
	img->Rect = r;
	img->_pix_fmt = PIXMAN_a8r8g8b8;
	return pixmanImage(img);
}

Image *
NewRGBA(Rectangle r)
{
	Image *img = allocImage();
	img->Rect = r;
	img->_pix_fmt = PIXMAN_r8g8b8a8;
	return pixmanImage(img);
}

Image *
NewARGB(Rectangle r)
{
	Image *img = allocImage();
	img->Rect = r;
	img->_pix_fmt = PIXMAN_a8r8g8b8;
	return pixmanImage(img);
}

void
ImageFree(Image *img)
{
	if (img->_pix)
		pixman_image_unref(img->_pix);
	free(img);
}

Image *
NewUniform(Color c)
{
	Image *img = allocImage();
	if (!img)
		return 0;
	img->Rect = Rect(-1E9, -1E9, 1E9, 1E9);
	pixman_color_t pc = ColorPixman(c);
	/* pixman_color_t pc = { 0xFFFF, 0x0000, 0x0000, 0xFFFF }; */
	img->_pix = pixman_image_create_solid_fill(&pc);
	return img;
}

void
ImageFill(Image *img, int r, int g, int b)
{
	pixman_color_t white = { r, g, b, 0xFFFF };
	pixman_image_t *fill = pixman_image_create_solid_fill(&white);
	Image *fillImg = allocImage();
	fillImg->_pix = fill;
	Draw(img, img->Rect, fillImg, ZP, PIXMAN_OP_SRC);
}


Color *
ImageAt(Image *img)
{
	return 0;
}

Alpha
ImageAlphaAt(Image *img)
{
	Color *c = ImageAt(img);
	return c->A;
}
Rectangle
ImageBounds(Image *img)
{
	return img->Rect;
}
