#ifndef XC_BACKEND_AUDIO
#define XC_BACKEND_AUDIO

#include <pulse/simple.h>

// set up pulse backend
#define backend_init pa_simple *s; \
pa_sample_spec ss; \
ss.format = PA_SAMPLE_S16NE; \
ss.channels = 2; \
ss.rate = SAMPLE_RATE; \
s = pa_simple_new(NULL, XMPCURSES, PA_STREAM_PLAYBACK, NULL, \
basename(MODULE), &ss, NULL, NULL, NULL) 
// pulse setup done

#define backend_write pa_simple_write(s, buffer, SAMPLE_RATE, 0)

#define backend_free pa_simple_free(s)

#endif
