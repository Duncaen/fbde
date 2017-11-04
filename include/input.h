
typedef struct Input {
	struct libinput *li;
	struct udev *udev;
	struct pollfd fds;
} Input;

extern int CursorX;
extern int CursorY;
