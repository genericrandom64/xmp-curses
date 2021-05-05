#ifndef XC_BACKEND
#define XC_BACKEND

#include <sndio.h>

// set up sndio
#define backend_init struct sio_hdl * s = sio_open(SIO_DEVANY, SIO_PLAY, 0); \
struct sio_par p; \
sio_initpar(&p); \
p.bits = 16; \
p.rate = SAMPLE_RATE; \
sio_setpar(s, &p); \
sio_start(s)
// sndio setup done. this is beautiful

#define backend_write sio_write(s, buffer, SAMPLE_RATE)

#define backend_free sio_close(s) 

#endif
