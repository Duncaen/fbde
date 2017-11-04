#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <time.h>
#include <err.h>

#include <pixman.h>

#include "draw.h"
#include "cursor.h"
#include "input.h"

static int exitit = 0;
static int fpscap = 120;

int
main()
{
	printf("alloc framebuffer\n");
	Image *fb = NewFramebufferImage("/dev/fb0");
	if (!fb)
		err(1, "NewFramebufferImage");

	// background and cursor overlay buffer
	printf("alloc bg and cursor surfaces\n");
	Image *bg = newargb(fb->r);
	Image *co = newargb(fb->r);

	// black out
	printf("alloc black pixel\n");
	Image *black = newuniform(Black);
	draw(fb, fb->r, black, ZP, Src);

	// background
	Point p2 = pt(rectdx(fb->r) << 16, rectdy(fb->r) << 16);
	GradientStop stops[] = {
		{ 0, { 0x0000, 0x0000, 0x4444, 0xdddd } },
		{ 0.45, { 0xeeee, 0x0000, 0x4444, 0xdddd } },
		{ 0.75, { 0xeeee, 0xeeee, 0x4444, 0xdddd } },
		{ 1, { 0xeeee, 0xeeee, 0x8888, 0xdddd } },
	};
	Image *gradient = NewLinearGradient(ZP, p2, stops, 4);
	draw(bg, bg->r, gradient, ZP, Over);

	// cursor
	Image *cur = newcursorp9(&arrow);
	Point center = ptdiv(rectsize(fb->r), 2);
	Point cursorPos = center;
	Point cursorLast= center;

	// draw background and cursor into memory and then to fb
	draw(co, co->r, bg, ZP, Src);
	draw(co, rectadd(cur->r, center), cur, ZP, Over);
	draw(fb, fb->r, co, ZP, Src);

	Input *in;
	if (!(in = NewLibInput()))
		return 1;

#define TIMEDIFF(t1, t2)	((t1.tv_sec-t2.tv_sec)*1000 + \
				(t1.tv_nsec-t2.tv_nsec)/1E6)

	struct timespec now, last, t1, t2;
	long deltatime = 0;
	clock_gettime(CLOCK_MONOTONIC, &last);
	Rectangle updR = fb->r;
	while (!InputNextEvent(in) && !exitit) {
		clock_gettime(CLOCK_MONOTONIC, &now);
		deltatime = TIMEDIFF(now, last);
		if (deltatime > 1000 / fpscap) {
			cursorPos.X += CursorX;
			cursorPos.Y += CursorY;
			if (cursorPos.X > rectdx(fb->r))
				cursorPos.X = rectdx(fb->r);
			if (cursorPos.X < 0)
				cursorPos.X = 0;
			if (cursorPos.Y > rectdy(fb->r))
				cursorPos.Y = rectdy(fb->r);
			if (cursorPos.Y < 0)
				cursorPos.Y = 0;
			CursorX = 0;
			CursorY = 0;
			last = now;

			updR = rectcanon(rectpt(cursorPos, cursorLast));
			updR = rectpt(updR.Min, ptadd(updR.Max,pt(16,16)));

			cursorLast = cursorPos;

			draw(co, updR, bg, updR.Min, Src);
			draw(co, rectadd(cur->r, cursorPos), cur, ZP, Over);
			draw(fb, updR, co, updR.Min, Src);
		}
		/* printf("sleep: %d\n",(1000/fpscap)-deltatime); */
		/* usleep(deltatime-(1000/fpscap)); */
	}
	return 0;
}
