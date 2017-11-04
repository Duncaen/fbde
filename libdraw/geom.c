#include <stdlib.h>
#include <stdio.h>

#include <pixman.h>

#include "draw.h"

const Point ZP = {0, 0};
const Point IP = {1E9, 1E9};
const Rectangle ZR = {{0, 0}, {0, 0}};
const Rectangle IR = {{-1E9, -1E9}, {1E9, 1E9}};

Point
pt(int x, int y)
{
	Point p;
	p.X = x;
	p.Y = y;
	return p;
}

int
ptstr(Point p, char **dst)
{
	return asprintf(dst, "(%d,%d)", p.X, p.Y);
}

Point
ptadd(Point p, Point q)
{
	p.X += q.X;
	p.Y += q.Y;
	return p;
}

Point
ptsub(Point p, Point q)
{
	p.X -= q.X;
	p.Y -= q.Y;
	return p;
}

Point
ptmul(Point p, int k)
{
	p.X *= k;
	p.Y *= k;
	return p;
}

Point
ptdiv(Point p, int k)
{
	p.X /= k;
	p.Y /= k;
	return p;
}

int
ptin(Point p, Rectangle r)
{
	return r.Min.X <= p.X && p.X < r.Max.X &&
	    r.Min.Y <= p.Y && p.Y < r.Max.Y;
}

int
pteq(Point p, Point q)
{
	return p.X == q.X && p.Y == q.Y;
}

Rectangle
rect(int x0, int y0, int x1, int y1)
{
	Rectangle r;
	int t;
	if (x0 > x1) {
		t = x0;
		x0 = x1;
		x1 = t;
	}
	if (y0 > y1) {
		t = y0;
		y0 = y1;
		y1 = t;
	}
	r.Min.X = x0;
	r.Min.Y = y0;
	r.Max.X = x1;
	r.Max.Y = y1;
	return r;
}

Rectangle
rectpt(Point min, Point max)
{
	Rectangle r;
	r.Min = min;
	r.Max = max;
	return r;
}

int
rectstr(Rectangle r, char **dst)
{
	return asprintf(dst, "(%d,%d)-(%d,%d)",
	    r.Min.X, r.Min.Y,
	    r.Max.X, r.Max.Y);
}

void
rectdump(Rectangle r)
{
	char *rs;
	rectstr(r, &rs);
	fputs(rs, stderr);
	free(rs);
}

int
rectdx(Rectangle r)
{
	return r.Max.X - r.Min.X;
}

int
rectdy(Rectangle r)
{
	return r.Max.Y - r.Min.Y;
}

Point
rectsize(Rectangle r)
{
	Point p;
	p.X = r.Max.X - r.Min.X;
	p.Y = r.Max.Y - r.Min.Y;
	return p;
}

Rectangle
rectadd(Rectangle r, Point p)
{
	r.Min.X += p.X;
	r.Min.Y += p.Y;
	r.Max.X += p.X;
	r.Max.Y += p.Y;
	return r;
}

Rectangle
rectsub(Rectangle r, Point p)
{
	r.Min.X -= p.X;
	r.Min.Y -= p.Y;
	r.Max.X -= p.X;
	r.Max.Y -= p.Y;
	return r;
}

Rectangle
rectinset(Rectangle r, int n)
{
	if (rectdx(r) < 2*n) {
		r.Min.X = (r.Min.X + r.Max.X) / 2;
		r.Max.X = r.Min.X;
	} else {
		r.Min.X = r.Min.X + n;
		r.Max.X = r.Max.X - n;
	}
	if (rectdy(r) < 2*n) {
		r.Min.Y = (r.Min.Y + r.Max.Y) / 2;
		r.Max.Y = r.Min.Y;
	} else {
		r.Min.Y = r.Min.Y + n;
		r.Max.Y = r.Max.Y - n;
	}
	return r;
}

int
rectempty(Rectangle r)
{
	return r.Min.X >= r.Max.X || r.Min.Y >= r.Max.Y;
}

int
recteq(Rectangle r, Rectangle s)
{
	return r.Min.X == s.Min.X &&
	    r.Max.X == s.Max.X &&
	    r.Min.Y == s.Min.Y &&
	    r.Max.Y == s.Max.Y;
}

int
rectoverlaps(Rectangle r, Rectangle s)
{
	return !rectempty(r) && !rectempty(s) &&
	    r.Min.X < s.Max.X && s.Min.X < r.Max.X &&
	    r.Min.Y < s.Max.Y && s.Min.Y < r.Max.Y;
}

int
rectin(Rectangle r, Rectangle s)
{
	if (rectempty(r))
		return 1;
	return s.Min.X <= r.Min.X &&
	    r.Max.X <= s.Max.X &&
	    s.Min.Y <= r.Min.Y &&
	    r.Max.Y <= r.Max.Y;
}

Rectangle
rectcanon(Rectangle r)
{
	int t;
	if (r.Max.X < r.Min.X) {
		t = r.Min.X;
		r.Min.X = r.Max.X;
		r.Max.X = t;
	}
	if (r.Max.Y < r.Min.Y) {
		t = r.Min.Y;
		r.Min.Y = r.Max.Y;
		r.Max.Y = t;
	}
	return r;
}

Rectangle
rectintersect(Rectangle r, Rectangle s)
{
	if (r.Min.X < s.Min.X)
		r.Min.X = s.Min.X;
	if (r.Min.Y < s.Min.Y)
		r.Min.Y = s.Min.Y;
	if (r.Max.X > s.Max.X)
		r.Max.X = s.Max.X;
	if (r.Max.Y > s.Max.Y)
		r.Max.Y = s.Max.Y;
	if (rectempty(r))
		return ZR;
	return r;
}

Rectangle
rectunion(Rectangle r, Rectangle s)
{
	if (rectempty(r))
		return s;
	if (rectempty(s))
		return r;
	if (r.Min.X > s.Min.X)
		r.Min.X = s.Min.X;
	if (r.Min.Y > s.Min.Y)
		r.Min.Y = s.Min.Y;
	if (r.Max.X > s.Max.X)
		r.Max.X = s.Max.X;
	if (r.Max.Y > s.Max.Y)
		r.Max.Y = s.Max.Y;
	return r;
}
