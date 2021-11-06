#ifndef XC_BACKEND_TERM
#define XC_BACKEND_TERM

struct termios t;
struct termios to;

void terminit() {
	printf("\033[?1049h\033[22;0;0t\033[?25l\x1B\x5B\x48\x1B\x5B\x4A");
	fcntl(0, F_SETFL, O_NONBLOCK);
	tcgetattr(0, &t);
	to = t;
	t.c_lflag &= ~ICANON;
	t.c_lflag &= ~ECHO;
	tcsetattr(0, TCSANOW, &t);
}

#define termend() { printf("\033[?25h\033[?1049l\033[23;0;0t"); tcsetattr(0, TCSANOW, &to); }
#define termret() printf("\033[1;1H%s", statusbar);
#define termerr() printf("\033[1;4H");

#endif
