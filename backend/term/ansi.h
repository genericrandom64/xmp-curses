#ifndef XC_BACKEND_TERM
#define XC_BACKEND_TERM

#define termend() printf("\033[?25h\033[?1049l\033[23;0;0t")
#define terminit() printf("\033[?1049h\033[22;0;0t\033[?25l\x1B\x5B\x48\x1B\x5B\x4A")


#endif
