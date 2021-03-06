#ifndef XC_LANG
#define XC_LANG

#define XMPCURSES "xmp-curses"
//#define XMPCURSES_HELP " __\n/  \\        _____________\n|  |       /             \\\n@  @       | It looks    |\n|| ||      | like you    |\n|| ||   <--| are having  |\n|\\_/|      | a stroke.   |\n\\___/      \\_____________/\n\ni do not want to make a help message. fuck you\n"

#define XMPCURSES_HELP "-R - loop all files\n"\
"-l - loop one file\n"\
"-s [0-100] - stereo separation\n"\
"-i [nearest|spline|linear] - interpolation\n"\
"-h you're reading this, you know\n"\
"-z [subsong number] - select subsong/sequence\n"\
"-D - run xmp-curses server that can be used with xmpcmd\n"

#define XMPCURSES_NOFILE "Please provide a module file.\n"
#define XMPCURSES_NEAREST "nearest"
#define XMPCURSES_SPLINE "spline"
#define XMPCURSES_UNKNOWN_ARG "cannot handle argument %c\n"
#define XMPCURSES_LOAD_FAIL "Could not load module.\n"
#define XMPCURSES_INT_ERR "Internal error\n"
#define XMPCURSES_STATUS_PAUSE 'P'
#define XMPCURSES_STATUS_LOOP 'L'
#define XMPCURSES_STATUS_LOOP_PLAYLIST 'R'

#endif
