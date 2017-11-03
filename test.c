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
static int fpscap = 60;

int
main()
{
	Image *fb = NewFramebufferImage("/dev/fb0");
	if (!fb)
		err(1, "NewFramebufferImage");

	// background and cursor overlay buffer
	Image *bg = NewARGB(fb->Rect);
	Image *co = NewARGB(fb->Rect);

	// black out
	Image *black = NewUniform(Black);
	Draw(fb, fb->Rect, black, ZP, Src);

	// background
	Point p2 = Pt(RectDx(fb->Rect) << 16, RectDy(fb->Rect) << 16);
	GradientStop stops[] = {
		{ 0, { 0x0000, 0x0000, 0x4444, 0xdddd } },
		{ 0.45, { 0xeeee, 0x0000, 0x4444, 0xdddd } },
		{ 0.75, { 0xeeee, 0xeeee, 0x4444, 0xdddd } },
		{ 1, { 0xeeee, 0xeeee, 0x8888, 0xdddd } },
	};
	Image *gradient = NewLinearGradient(ZP, p2, stops, 4);
	Draw(bg, bg->Rect, gradient, ZP, Over);

	// cursor
	Image *cur = NewPlan9CursorImage(&arrow);
	Point center = PointDiv(RectSize(fb->Rect), 2);
	Point cursorPos = center;
	Point cursorLast= center;

	// draw background and cursor into memory and then to fb
	Draw(co, co->Rect, bg, ZP, Src);
	Draw(co, RectAdd(cur->Rect, center), cur, ZP, Over);
	Draw(fb, fb->Rect, co, ZP, Src);

	Input *in;
	if (!(in = NewLibInput()))
		return 1;

#define TIMEDIFF(t1, t2)	((t1.tv_sec-t2.tv_sec)*1000 + \
				(t1.tv_nsec-t2.tv_nsec)/1E6)

	struct timespec now, last, t1, t2;
	long deltatime = 0;
	clock_gettime(CLOCK_MONOTONIC, &last);
	Rectangle updR = fb->Rect;
	while (!InputNextEvent(in) && !exitit) {
		clock_gettime(CLOCK_MONOTONIC, &now);
		deltatime = TIMEDIFF(now, last);
		if (deltatime > 1000 / fpscap) {
			cursorPos.X += CursorX;
			cursorPos.Y += CursorY;
			if (cursorPos.X > RectDx(fb->Rect))
				cursorPos.X = RectDx(fb->Rect);
			if (cursorPos.X < 0)
				cursorPos.X = 0;
			if (cursorPos.Y > RectDy(fb->Rect))
				cursorPos.Y = RectDy(fb->Rect);
			if (cursorPos.Y < 0)
				cursorPos.Y = 0;
			CursorX = 0;
			CursorY = 0;
			last = now;

			updR = RectCanon(RectPt(cursorPos, cursorLast));
			updR = RectPt(updR.Min, PointAdd(updR.Max,Pt(16,16)));

			cursorLast = cursorPos;

			Draw(co, updR, bg, updR.Min, Src);
			Draw(co, RectAdd(cur->Rect, cursorPos), cur, ZP, Over);
			Draw(fb, updR, co, updR.Min, Src);
		}
	}
	return 0;
}
