/*
Image *
FarbfeldDecode(char *buf)
{
	int w, h;
	Image *img;
	char *bits;

	if (memcmp(buf, "farbfeld", 8))
		return 0

	w = ntohl(*(uint32_t *)&buf[8]);
	h = ntohl(*(uint32_t *)&buf[12]);

	bits = calloc(w * h, strlen("888"));
	if (!bits)
		return 0;

	// PIXMAN_r8g8b8a8
	img = allocImage();
	img->r = r;
	img->_pix_fmt = PIXMAN_a8;
	img->_stride = 8;
	return pixmanImage(img);
}
*/
