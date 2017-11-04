#include <sys/ioctl.h>
#include <sys/mman.h>

#include <linux/fb.h>

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <err.h>

#include <pixman.h>

#include "draw.h"

typedef struct Framebuffer {
	const char *Path;
	int Fd;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	Rectangle r;
	Rectangle clipr;
	int bpp;
	void *mem;
	void *Pix;
	size_t len;
} Framebuffer;

#define MIN(a, b)	((a) < (b) ? (a) : (b))
#define MAX(a, b)	((a) > (b) ? (a) : (b))
#define NLEVELS		(1 << 8)

static int nr, ng, nb;			/* color levels */
static int rl, rr, gl, gr, bl, br;	/* shifts per color */

static void fb_cmap_save(Framebuffer *fb, int save)
{
	static unsigned short red[NLEVELS], green[NLEVELS], blue[NLEVELS];
	struct fb_cmap cmap;
	if (fb->finfo.visual == FB_VISUAL_TRUECOLOR)
		return;
	cmap.start = 0;
	cmap.len = MAX(nr, MAX(ng, nb));
	cmap.red = red;
	cmap.green = green;
	cmap.blue = blue;
	cmap.transp = NULL;
	ioctl(fb->Fd, save ? FBIOGETCMAP : FBIOPUTCMAP, &cmap);
}

void fb_cmap(Framebuffer *fb)
{
	unsigned short red[NLEVELS], green[NLEVELS], blue[NLEVELS];
	struct fb_cmap cmap;
	int i;
	if (fb->finfo.visual == FB_VISUAL_TRUECOLOR)
		return;

	for (i = 0; i < nr; i++)
		red[i] = (65535 / (nr - 1)) * i;
	for (i = 0; i < ng; i++)
		green[i] = (65535 / (ng - 1)) * i;
	for (i = 0; i < nb; i++)
		blue[i] = (65535 / (nb - 1)) * i;

	cmap.start = 0;
	cmap.len = MAX(nr, MAX(ng, nb));
	cmap.red = red;
	cmap.green = green;
	cmap.blue = blue;
	cmap.transp = NULL;

	ioctl(fb->Fd, FBIOPUTCMAP, &cmap);
}

static void init_colors(Framebuffer *fb)
{
	nr = 1 << fb->vinfo.red.length;
	ng = 1 << fb->vinfo.blue.length;
	nb = 1 << fb->vinfo.green.length;
	rr = 8 - fb->vinfo.red.length;
	rl = fb->vinfo.red.offset;
	gr = 8 - fb->vinfo.green.length;
	gl = fb->vinfo.green.offset;
	br = 8 - fb->vinfo.blue.length;
	bl = fb->vinfo.blue.offset;
}

Framebuffer *
NewFramebuffer(char *path)
{
	int fd = -1;
	Framebuffer *fb;

	if (!(fb = malloc(sizeof (Framebuffer))))
		return 0;

	fb->Path = path ? path : "/dev/fb0";
	if ((fd = open(fb->Path, O_RDWR)) < 0)
		return 0;
	if (ioctl(fd, FBIOGET_VSCREENINFO, &fb->vinfo) < 0)
		return 0;
	if (ioctl(fd, FBIOGET_FSCREENINFO, &fb->finfo) < 0)
		return 0;
	fcntl(fd, F_SETFD, fcntl(fd, F_GETFD)|FD_CLOEXEC);
	fb->Fd = fd;

	fb->r = rect(0, 0, fb->vinfo.xres, fb->vinfo.yres);
	fb->clipr = rectpt(pt(fb->vinfo.xoffset, fb->vinfo.yoffset), rectsize(fb->r));
	fb->len = fb->finfo.line_length * fb->vinfo.yres_virtual;
	fb->mem = mmap(0, fb->len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	fb->bpp = (fb->vinfo.bits_per_pixel + 7) >> 3;
	fb->Pix = fb->mem + fb->vinfo.yoffset * fb->finfo.line_length +
	    fb->vinfo.xoffset * fb->bpp;

	init_colors(fb);
	fb_cmap_save(fb, 1);
	fb_cmap(fb);

	fbdump(fb);

	return fb;
}

void
fbdump(Framebuffer *fb)
{
	fprintf(stderr, "framebuffer(%p): ");
	fputs("r=", stderr);
	rectdump(fb->r);
	fputs(" clipr=", stderr);
	rectdump(fb->clipr);
	putc('\n', stderr);
}

Image *
NewFramebufferImage(char *path)
{
	Framebuffer *fb;
	Image *img;
	if (!(fb = NewFramebuffer(path)))
		return 0;
	img = newimaged(fb->r, PIXMAN_a8r8g8b8, 0, 0, fb->Pix);
	if (!img)
		return 0;
	return img;
}
