typedef struct Cursor {
	Point offset;
	unsigned int clr[2*16];
	unsigned int set[2*16];
} Cursor;

extern Cursor arrow;

Image *newcursorp9(Cursor *);
