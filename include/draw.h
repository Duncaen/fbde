typedef struct Point {
	int X;
	int Y;
} Point;

typedef struct Rectangle {
	Point Min;
	Point Max;
} Rectangle;

typedef struct RGBA {
	unsigned int R, G, B, A;
} RGBA;

typedef unsigned int Alpha;
typedef RGBA Color;

typedef struct Image {
	Rectangle r;
	Rectangle clipr;
	void *Pix;
	unsigned long chan;
	Color *fill;
	int repl;

#ifndef NO_PIXMAN
	void *_pixman_image;
	/* pixman_format_code_t _pix_fmt; */
	/* pixman_image_t *_pix; */
#endif
} Image;

typedef struct GradientStop {
	double X;
	Color Color;
} GradientStop;

typedef enum Op {
	Over,
	Src,
	Add,
} Op;

typedef struct Drawer {
	int (*imagealloc)(Image *);
	void (*imagefree)(Image *);
	void (*drawmask)(Image *, Rectangle, Image *, Point, Image *, Point, Op);
	Image *(*lineargradient)(Point, Point, GradientStop *, int);
} Drawer;

extern Drawer *drawer;

/* color.c */
static Color Red = { 0xFFFF, 0x0000, 0x0000, 0xFFFF };
static Color White = { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };
static Color Black = { 0x0000, 0x0000 , 0x0000, 0xFFFF };

/* geom.c */
extern const Point ZP;
extern const Point IP;
extern const Rectangle ZR;
extern const Rectangle IR;

Point	pt(int, int);
int	ptstr(Point, char **);
Point	ptadd(Point, Point);
Point	ptsub(Point, Point);
Point	ptmul(Point, int);
Point	ptdiv(Point, int);
int	ptin(Point, Rectangle);
int	pteq(Point, Point);

Rectangle	rect(int, int, int, int);
Rectangle	rectpt(Point, Point);
int		rectstr(Rectangle, char **);
void		rectdump(Rectangle);
int		rectdx(Rectangle);
int		rectdy(Rectangle);
Point		rectsize(Rectangle);
Rectangle	rectadd(Rectangle, Point);
Rectangle	rectsub(Rectangle, Point);
Rectangle	rectinset(Rectangle, int);
int		rectempty(Rectangle);
int		recteq(Rectangle, Rectangle);
int		rectoverlaps(Rectangle, Rectangle);
int		rectin(Rectangle, Rectangle);
Rectangle	rectcanon(Rectangle);
Rectangle	rectintersect(Rectangle, Rectangle);
Rectangle	rectunion(Rectangle, Rectangle);

/* draw.c */
void drawmask(Image *, Rectangle, Image *, Point, Image *, Point, Op);
void draw(Image *, Rectangle, Image *, Point, Op);

/* image.c */
Image *newimaged(Rectangle, unsigned long, int, Color *, void *);
Image *newimage(Rectangle, unsigned long, int, Color *);
Image *newalpha(Rectangle);
Image *newalpha16(Rectangle);
Image *newargb(Rectangle);
Image *newrgba(Rectangle);
Image *newuniform(Color);
void freeimg(Image *);

/* gradient.c */
Image *NewLinearGradient(Point, Point, GradientStop *, int);

/* pixman.c */
#ifndef NO_PIXMAN
extern const int OpPixman[];
Image *lineargradientpixman(Point, Point, GradientStop *, int);
void drawmaskpixman(Image *, Rectangle, Image *, Point, Image *, Point, Op);
#endif
