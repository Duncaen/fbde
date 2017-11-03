#include <stdlib.h>

#include <pixman.h>

#include "draw.h"

pixman_gradient_stop_t *
GradientStopPixman(GradientStop *stops, int n)
{
	pixman_gradient_stop_t *pix_stops;
	pix_stops = calloc(n, sizeof (pixman_gradient_stop_t));
	if (!pix_stops)
		return 0;

	for (int i = 0; i < n; i++) {
		pix_stops[i].x = pixman_double_to_fixed(stops[i].X);
		/* printf("GradientStop: x=%d\n", pix_stops[i].x); */
		pix_stops[i].color = ColorPixman(stops[i].Color);
	}
	return pix_stops;
}

Image *
NewLinearGradient(Point p1, Point p2,
    GradientStop *stops, int n)
{
	pixman_point_fixed_t pix_p1, pix_p2;
	pixman_gradient_stop_t *pix_stops;
	Image *img;

	img = allocImage();
	if (!img)
		return 0;
	img->Rect = Rect(-1E9, -1E9, 1E9, 1E9);

	pix_p1 = PointPixman(p1);
	pix_p2 = PointPixman(p2);
	pix_stops = GradientStopPixman(stops, n);

	img->_pix = pixman_image_create_linear_gradient(
	    &pix_p1, &pix_p2, pix_stops, n);

	return img;
}
