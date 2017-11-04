#include <stdlib.h>

#include <pixman.h>

#include "draw.h"
#include "cursor.h"

Cursor arrow = {
	{ -1, -1 },
	{
		0xFF, 0xFF, 0x80, 0x01, 0x80, 0x02, 0x80, 0x0C,
		0x80, 0x10, 0x80, 0x10, 0x80, 0x08, 0x80, 0x04,
		0x80, 0x02, 0x80, 0x01, 0x80, 0x02, 0x8C, 0x04,
		0x92, 0x08, 0x91, 0x10, 0xA0, 0xA0, 0xC0, 0x40,
	},
	{
		0x00, 0x00, 0x7F, 0xFE, 0x7F, 0xFC, 0x7F, 0xF0,
		0x7F, 0xE0, 0x7F, 0xE0, 0x7F, 0xF0, 0x7F, 0xF8,
		0x7F, 0xFC, 0x7F, 0xFE, 0x7F, 0xFC, 0x73, 0xF8,
		0x61, 0xF0, 0x60, 0xE0, 0x40, 0x40, 0x00, 0x00,
	},
};

Image *
newcursorp9(Cursor *cur)
{
	unsigned char *ip, *mp;
	Image *fill, *mask, *bord, *img, *white, *black;
	int i, j, set, clr;
	Rectangle curr;

	curr = rect(0, 0, 16, 16);
	img = newrgba(curr);
	fill = newalpha(curr);
	mask = newalpha(curr);
	bord = newalpha(curr);
	white = newuniform(White);
	black = newuniform(Black);

	ip = fill->Pix;
	mp = mask->Pix;

	for (i = 0; i < 32; i++) {
		set = cur->set[i];
		clr = cur->clr[i];
		for (j = 0x80; j; j>>=1) {
			*ip++ = set&j ? 0x00 : 0xFF;
			*mp++ = (clr|set)&j ? 0xFF : 0x00;
		}
	}

	drawmask(bord, img->r, fill, ZP, mask, ZP, Src);
	drawmask(img, img->r, black, ZP, mask, ZP, Src);
	drawmask(img, img->r, white, ZP, bord, ZP, Over);

	freeimg(fill);
	freeimg(mask);
	freeimg(bord);
	freeimg(white);
	freeimg(black);

	return img;
}
