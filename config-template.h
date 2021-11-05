// choose a language
// the first one defined will be used
#include "lang/en-us.h"

// choose a backend
// the first one defined will be used
#ifdef XC_WANT_BACKEND_AUDIO
#include "backend/audio/AUDIOSEL.h"
#endif

// choose a terminal backend
// the first one defined will be used
#ifdef XC_WANT_BACKEND_TERM
#include "backend/term/TERMSEL.h"
#endif

// keybinds

#define XMPCURSES_QUIT 'q'

#define XMPCURSES_PAUSE ' '

#define XMPCURSES_LOOP 'L'
#define XMPCURSES_LOOP_PLAYLIST 'R'

#define XMPCURSES_MOD_NEXT 'k'
#define XMPCURSES_MOD_PREV 'j'

#define XMPCURSES_POS_NEXT 'l'
#define XMPCURSES_POS_PREV 'h'

#define SHM "/xmp-curses.shm"
