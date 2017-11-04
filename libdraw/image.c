#include <stdlib.h>

#include <pixman.h>

#include "draw.h"

Image *
newimaged(Rectangle r, unsigned long chan, int repl, Color *fill, void *data)
{
	Image *img;
	if (!(img = malloc(sizeof (Image))))
		return 0;
	img->r = r;
	img->clipr = (repl && fill) ? rect(0,0,1,1) : r;
	img->chan = chan;
	img->repl = repl;
	img->fill = fill;
	img->Pix = data;
	if (drawer->imagealloc(img))
		return 0;
	return img;
}

Image *
newimage(Rectangle r, unsigned long chan, int repl, Color *fill)
{
	return newimaged(r, chan, repl, fill, 0);
}

void
freeimg(Image *img)
{
	drawer->imagefree(img);
	free(img);
}

Image *
newalpha(Rectangle r)
{
	return newimage(r, PIXMAN_a8, 0, 0);
}

Image *
newalpha16(Rectangle r)
{
	return newimage(r, PIXMAN_a8r8g8b8, 0, 0);
}

Image *
newrgba(Rectangle r)
{
	return newimage(r, PIXMAN_r8g8b8a8, 0, 0);
}

Image *
newargb(Rectangle r)
{
	return newimage(r, PIXMAN_a8r8g8b8, 0, 0);
}

Image *
newuniform(Color c)
{
	return newimage(IR, PIXMAN_a8r8g8b8, 1, &c);
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
	return img->r;
}
